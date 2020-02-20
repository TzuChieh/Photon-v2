from ..utility import (
        settings,
        blender)
from . import render
from . import export

import bpy
from bl_ui import (
        properties_output,
        properties_data_camera)

import uuid
import tempfile
from pathlib import Path
import shutil
import select
import socket
import sys
import time


class PhPhotonRenderEngine(bpy.types.RenderEngine):
    # These three members are used by blender to set up the
    # RenderEngine; define its internal name, visible name and capabilities.
    bl_idname = settings.renderer_id_name
    bl_label = "Photon"
    bl_use_preview = False

    # Init is called whenever a new render engine instance is created. Multiple instances may exist at the same time,
    # for example for a viewport and final render.
    def __init__(self):
        super().__init__()

        self.renderer = render.RenderProcess()
        self.identifier = str(uuid.uuid4())
        self.renderer_data_path = None

        print("Photon Renderer started (id: %s)" % self.identifier)

    # When the render engine instance is destroy, this is called. Clean up any render engine data here, for example
    # stopping running render threads.
    def __del__(self):
        # HACK: blender seems to be calling __del__ even if __init__ is not called first, filtering this situation out
        if not hasattr(self, "renderer") or not hasattr(self, "identifier") or not hasattr(self, "renderer_data_path"):
            return

        if self.renderer.is_running():
            self.renderer.exit()

        # Remove all generated data on disk
        if self.renderer_data_path is not None:
            if self.renderer_data_path.exists():
                shutil.rmtree(self.renderer_data_path)

        print("Photon Renderer exited (id: %s)" % self.identifier)

    # This is the method called by Blender for both final renders (F12) and small preview for materials, world
    # and lights.
    def render(self, b_depsgraph):
        b_scene = b_depsgraph.scene
        width_px = blender.get_render_width_px(b_scene)
        height_px = blender.get_render_height_px(b_scene)
        renderer_data_path = self._get_temp_folder_path()
        scene_file_name = "__temp_scene"
        image_file_name = "__temp_rendered"
        image_file_format = "exr"
        refresh_seconds = 2

        self.renderer_data_path = renderer_data_path

        exporter = export.Exporter(str(renderer_data_path.resolve()))
        exporter.begin(scene_file_name)
        exporter.export_core_commands(b_scene)
        exporter.export(b_depsgraph)
        exporter.end()

        scene_file_path = renderer_data_path / (scene_file_name + ".p2")
        self.renderer.set_scene_file_path("\"" + str(scene_file_path.resolve()) + "\"")

        image_file_path = renderer_data_path / image_file_name
        self.renderer.set_image_output_path("\"" + str(image_file_path.resolve()) + "\"")
        self.renderer.set_image_format(image_file_format)
        self.renderer.request_raw_output()

        # self.renderer.request_intermediate_output(interval=refresh_seconds, unit='s', is_overwriting=True)
        intermediate_image_file_path = Path(str(image_file_path) + "_intermediate_")
        intermediate_image_file_path = intermediate_image_file_path.with_suffix("." + image_file_format)

        self.renderer.set_num_render_threads(b_scene.render.threads)

        HOST = '127.0.0.1'  # The server's hostname or IP address
        PORT = 7000  # The port used by the server
        self.renderer.set_port(PORT)

        self.renderer.run()

        b_render_result = self.begin_result(0, 0, width_px, height_px)
        b_render_layer = b_render_result.layers[0]

        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
            s.settimeout(refresh_seconds)

            # Connect to the rendering server
            # max_retries = 100
            max_retries = 1000
            num_retries = 0
            is_connected = False
            while num_retries < max_retries:
                try:
                    s.connect((HOST, PORT))

                    is_connected = True
                    print("note: connected to the rendering server")
                    break
                except OSError:
                    print("note: connection failed, retrying... (attempt %d/%d)" % (num_retries + 1, max_retries))
                    num_retries += 1
                    time.sleep(refresh_seconds)

            if is_connected:
                s.setblocking(False)

                # Connection established

                # Keep receiving data until program terminated or connection ended
                num_chunk_bytes = 1 * 1024 * 1024
                data = bytes()
                num_image_bytes = -1
                while self.renderer.is_running():
                    try:
                        ready_for_read, ready_for_write, in_error = select.select([s], [], [], 5)
                    except select.error:
                        # 0 = done receiving, 1 = done sending, 2 = both
                        s.shutdown(2)
                        # TODO: maybe reconnect is needed on some error type
                        print("note: connection to the rendering server ended")
                        break

                    if ready_for_read:
                        data += s.recv(num_chunk_bytes)

                        # Data for the image size received
                        if num_image_bytes < 0 and len(data) >= 8:
                            num_image_bytes = int.from_bytes(data[:8], sys.byteorder)

                        # Data for the image received
                        if num_image_bytes > 0 and len(data) >= 8 + num_image_bytes:
                            with open(intermediate_image_file_path, 'wb') as image_file:
                                image_file.write(data[8:8 + num_image_bytes])

                            b_render_layer.load_from_file(str(intermediate_image_file_path.resolve()))
                            self.update_result(b_render_result)

                            # Chop current image data off
                            data = data[8 + num_image_bytes:]

                            # Reset image size to an unset state
                            num_image_bytes = -1

                            time.sleep(refresh_seconds)
            else:
                print("warning: connection failed")

        image_file_path = image_file_path.with_suffix("." + image_file_format)
        b_render_layer.load_from_file(str(image_file_path.resolve()))
        self.end_result(b_render_result)

        self.renderer.exit()

    # For viewport renders, this method gets called once at the start and whenever the scene or 3D viewport changes.
    # This method is where data should be read from Blender in the same thread. Typically a render thread will be
    # started to do the work while keeping Blender responsive.
    def view_update(self, b_context, b_depsgraph):
        print("view_update")

    # For viewport renders, this method is called whenever Blender redraws the 3D viewport. The renderer is expected to
    # quickly draw the render with OpenGL, and not perform other expensive work. Blender will draw overlays for
    # selection and editing on top of the rendered image automatically.
    def view_draw(self, b_context, b_depsgraph):
        print("view_draw")

    def _get_temp_folder_path(self):
        folder_name = "__photon_temp_" + self.identifier

        blend_file_folder_path = bpy.path.abspath("//")
        if bpy.data.is_saved and blend_file_folder_path:
            folder_path = Path(blend_file_folder_path)
        else:
            folder_path = Path(tempfile.gettempdir())

        return folder_path / folder_name


class PhRenderPanel(bpy.types.Panel):
    bl_space_type = "PROPERTIES"
    bl_region_type = "WINDOW"
    bl_context = "render"

    COMPATIBLE_ENGINES = {settings.renderer_id_name}

    @classmethod
    def poll(cls, context):
        render_settings = context.scene.render
        return render_settings.engine in cls.COMPATIBLE_ENGINES


class PH_RENDERING_PT_rendering(PhRenderPanel):
    bl_label = "PR: Rendering"

    bpy.types.Scene.ph_render_integrator_type = bpy.props.EnumProperty(
        items=[
            ("BVPT", "Pure Path Tracing", "Slow but versatile"),
            ("BNEEPT", "NEE Path Tracing", "Similar to pure PT but good on rendering small lights"),
            ("VPM", "Photon Mapping", "Rough preview, fairly good at caustics"),
            ("PPM", "Progressive Photon Mapping", "Good at complex lighting condition"),
            ("SPPM", "Stochastic Progressive Photon Mapping", "Good at complex lighting condition"),
            ("BVPTDL", "Pure Path Tracing (Direct Lighting)", ""),
            ("ATTRIBUTE", "Attribute", ""),
            ("CUSTOM", "Custom", "Directly input SDL commands for renderer.")
        ],
        name="Rendering Method",
        description="Photon-v2's rendering methods",
        default="BNEEPT"
    )

    bpy.types.Scene.ph_scheduler_type = bpy.props.EnumProperty(
        items=[
            ('bulk', "Bulk", ""),
            ('stripe', "Stripe", ""),
            ('grid', "Grid", ""),
            ('tile', "Tile", ""),
            ('spiral', "Spiral", ""),
            ('spiral-grid', "Spiral-grid", "")
        ],
        name="Scheduler",
        description="Order of rendering for pixels",
        default='spiral-grid'
    )

    bpy.types.Scene.ph_render_num_photons = bpy.props.IntProperty(
        name="Number of Photons",
        description="Number of photons used.",
        default=200000,
        min=1
    )

    bpy.types.Scene.ph_render_num_spp_pm = bpy.props.IntProperty(
        name="Samples per Pixel",
        description="Number of samples per pixel.",
        default=4,
        min=1
    )

    bpy.types.Scene.ph_render_num_passes = bpy.props.IntProperty(
        name="Number of Passes",
        description="Number of rendering passes.",
        default=40,
        min=1
    )

    bpy.types.Scene.ph_render_kernel_radius = bpy.props.FloatProperty(
        name="Photon Radius",
        description="larger radius results in blurrier images",
        default=0.1,
        min=0
    )

    bpy.types.Scene.ph_use_crop_window = bpy.props.BoolProperty(
        name="Use Crop Window",
        description="",
        default=False
    )

    bpy.types.Scene.ph_crop_min_x = bpy.props.IntProperty(
        name="Min X",
        description="",
        default=0,
        min=0
    )

    bpy.types.Scene.ph_crop_min_y = bpy.props.IntProperty(
        name="Min Y",
        description="",
        default=0,
        min=0
    )

    bpy.types.Scene.ph_crop_width = bpy.props.IntProperty(
        name="Width",
        description="",
        default=1,
        min=1
    )

    bpy.types.Scene.ph_crop_height = bpy.props.IntProperty(
        name="Height",
        description="",
        default=1,
        min=1
    )

    bpy.types.Scene.ph_render_custom_sdl = bpy.props.StringProperty(
        name="SDL Command",
        description="",
        default=""
    )

    def draw(self, b_context):
        b_scene = b_context.scene
        b_layout = self.layout

        b_layout.prop(b_scene, "ph_render_integrator_type")

        render_method = b_scene.ph_render_integrator_type
        if render_method == "BVPT" or render_method == "BNEEPT" or render_method == "BVPTDL" or render_method == "ATTRIBUTE":
            b_layout.prop(b_scene, "ph_render_num_spp")
            b_layout.prop(b_scene, "ph_render_sample_filter_type")
            if render_method != "ATTRIBUTE":
                b_layout.prop(b_scene, "ph_scheduler_type")
        elif render_method == "VPM" or render_method == "PPM" or render_method == "SPPM":
            b_layout.prop(b_scene, "ph_render_num_photons")
            b_layout.prop(b_scene, "ph_render_num_spp_pm")
            b_layout.prop(b_scene, "ph_render_num_passes")
            b_layout.prop(b_scene, "ph_render_kernel_radius")
        elif render_method == "CUSTOM":
            b_layout.prop(b_scene, "ph_render_custom_sdl")
        else:
            pass

        if render_method != "CUSTOM":
            b_layout.prop(b_scene, "ph_use_crop_window")

        use_crop_window = b_scene.ph_use_crop_window
        if use_crop_window and render_method != "CUSTOM":
            b_layout.prop(b_scene, "ph_crop_min_x")
            b_layout.prop(b_scene, "ph_crop_min_y")
            b_layout.prop(b_scene, "ph_crop_width")
            b_layout.prop(b_scene, "ph_crop_height")


class PH_RENDERING_PT_sampling(PhRenderPanel):
    bl_label = "PR: Sampling"

    bpy.types.Scene.ph_render_num_spp = bpy.props.IntProperty(
        name="Samples per Pixel",
        description="Number of samples used for each pixel.",
        default=40,
        min=1,
        max=2 ** 31 - 1,
    )

    bpy.types.Scene.ph_render_sample_filter_type = bpy.props.EnumProperty(
        items=[
            ("BOX", "Box", "box filter"),
            ("GAUSSIAN", "Gaussian", "Gaussian filter"),
            ("MN", "Mitchell-Netravali", "Mitchell-Netravali filter"),
            ("BH", "Blackman-Harris", "Blackman-Harris filter")
        ],
        name="Sample Filter",
        description="Photon-v2's sample filter types",
        default="BH"
    )

    bpy.types.Scene.ph_render_sample_generator_type = bpy.props.EnumProperty(
        items=[
            ("RANDOM", "Random", "Pseudorandom numbers"),
            ("STRATIFIED", "Stratified", "Quasirandom numbers based on domain subdivision"),
            ("HALTON", "Halton", "Quasirandom numbers that are well distributed over all dimensions")
        ],
        name="Sample Generator",
        description="Photon-v2's sample generator types",
        default="STRATIFIED"
    )

    def draw(self, b_context):
        b_scene = b_context.scene
        layout = self.layout

        layout.prop(b_scene, "ph_render_num_spp")
        layout.prop(b_scene, "ph_render_sample_filter_type")
        layout.prop(b_scene, "ph_render_sample_generator_type")


class PH_RENDERING_PT_data_structures(PhRenderPanel):
    bl_label = "PR: Data Structures"

    bpy.types.Scene.ph_top_level_accelerator = bpy.props.EnumProperty(
        items=[
            ('BF', "Brute Force", "Iteratively test every object. Super-duper SLOW."),
            ('BVH', "BVH", "Bounding volume hierarchy."),
            ('IKD', "Indexed Kd-tree", "Indexed Kd-tree.")
        ],
        name="Top Level Accelerator",
        description="Type of the first level of acceleration structures.",
        default='BVH'
    )

    def draw(self, b_context):
        b_scene = b_context.scene
        b_layout = self.layout

        b_layout.prop(b_scene, "ph_top_level_accelerator")


# class PH_RENDERING_PT_options(PhRenderPanel):
#     bl_label = "PR: Options"
#
#     bpy.types.Scene.ph_use_cycles_material = bpy.props.BoolProperty(
#         name="Use Cycles Material",
#         description="render/export the scene with materials converted from Cycles to Photon",
#         default=False
#     )
#
#     def draw(self, context):
#         scene = context.scene
#         layout = self.layout
#
#         layout.prop(scene, "ph_use_cycles_material")


RENDER_PANEL_CLASSES = [
    PH_RENDERING_PT_sampling,
    # PH_RENDERING_PT_options,
    PH_RENDERING_PT_rendering,
    PH_RENDERING_PT_data_structures
]


class RendererModule(blender.BlenderModule):
    def register(self):
        # Register the render engine.
        bpy.utils.register_class(PhPhotonRenderEngine)

        # RenderEngines also need to tell UI Panels that they are compatible;
        # otherwise most of the UI will be empty when the engine is selected.

        properties_output.RENDER_PT_dimensions.COMPAT_ENGINES.add(PhPhotonRenderEngine.bl_idname)

        properties_data_camera.DATA_PT_lens.COMPAT_ENGINES.add(PhPhotonRenderEngine.bl_idname)
        properties_data_camera.DATA_PT_camera.COMPAT_ENGINES.add(PhPhotonRenderEngine.bl_idname)

        # properties_data_light.DATA_PT_light.COMPAT_ENGINES.add(PhotonRenderer.bl_idname)
        # properties_data_light.DATA_PT_area.COMPAT_ENGINES.add(PhotonRenderer.bl_idname)

        # properties_material.MATERIAL_PT_preview.COMPAT_ENGINES.add(PhotonRenderer.bl_idname)

        for clazz in RENDER_PANEL_CLASSES:
            bpy.utils.register_class(clazz)

    def unregister(self):
        bpy.utils.unregister_class(PhPhotonRenderEngine)

        properties_output.RENDER_PT_dimensions.COMPAT_ENGINES.remove(PhPhotonRenderEngine.bl_idname)

        properties_data_camera.DATA_PT_lens.COMPAT_ENGINES.remove(PhPhotonRenderEngine.bl_idname)
        properties_data_camera.DATA_PT_camera.COMPAT_ENGINES.remove(PhPhotonRenderEngine.bl_idname)

        # properties_data_light.DATA_PT_light.COMPAT_ENGINES.remove(PhotonRenderer.bl_idname)
        # properties_data_light.DATA_PT_area.COMPAT_ENGINES.remove(PhotonRenderer.bl_idname)

        # properties_material.MATERIAL_PT_preview.COMPAT_ENGINES.remove(PhotonRenderer.bl_idname)

        for clazz in RENDER_PANEL_CLASSES:
            bpy.utils.unregister_class(clazz)


def include_module(module_manager):
    module_manager.add_module(RendererModule())
