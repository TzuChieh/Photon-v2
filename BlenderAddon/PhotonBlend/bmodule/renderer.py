from utility import settings, blender
from bmodule import render
from bmodule import export

import bpy
from bl_ui import (
    properties_render,
    properties_output,
    properties_data_camera,
    )

import uuid
import tempfile
from pathlib import Path
import shutil
import select
import socket
import sys
import time


@blender.register_class
class PhPhotonRenderEngine(bpy.types.RenderEngine):
    # These three members are used by blender to set up the
    # RenderEngine; define its internal name, visible name and capabilities.
    bl_idname = settings.render_engine_idname
    bl_label = "Photon"
    bl_use_preview = False

    # Do not expose Cycles and Eevee shading nodes in the node editor user interface, so own nodes can be used instead.
    bl_use_shading_nodes_custom = True

    # Init is called whenever a new render engine instance is created. Multiple instances may exist at the same 
    # time, for example for a viewport and final render.
    def __init__(self):
        super().__init__()

        self.renderer = render.RenderProcess()
        self.identifier = str(uuid.uuid4())
        self.renderer_data_path = self._get_temp_folder_path(self.identifier)
        self.scene_file_path = None

        print("Photon render engine started (id: %s)" % self.identifier)

    # When the render engine instance is destroyed, this is called. Clean up any render engine data here, 
    # for example stopping running render threads.
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

        print("Photon render engine exited (id: %s)" % self.identifier)

    # Export scene data for render
    def update(self, b_blend_data, b_depsgraph):
        b_scene = b_depsgraph.scene_eval

        scene_file_name = "__temp_scene"

        exporter = export.Exporter(str(self.renderer_data_path))
        exporter.begin(scene_file_name)
        exporter.export_core_commands(b_scene)
        exporter.export(b_depsgraph)
        exporter.export_options(b_scene)
        exporter.end()

        self.scene_file_path = self.renderer_data_path / (scene_file_name + ".p2")

    # This is the method called by Blender for both final renders (F12) and small preview for materials, world
    # and lights.
    def render(self, b_depsgraph):
        if not self.scene_file_path:
            print("ERROR: no scene file is generated")
            return

        b_scene = b_depsgraph.scene_eval
        width_px = blender.get_render_width_px(b_scene)
        height_px = blender.get_render_height_px(b_scene)
        
        image_file_name = "__temp_rendered"
        image_file_format = "exr"
        image_file_path = self.renderer_data_path / image_file_name
        poll_seconds = 1.0

        self.renderer.set_scene_file_path(self.scene_file_path)
        self.renderer.set_image_output_path(image_file_path)
        self.renderer.set_image_format(image_file_format)
        self.renderer.request_raw_output()

        # self.renderer.request_intermediate_output(interval=refresh_seconds, unit='s', is_overwriting=True)
        intermediate_image_file_path = Path(str(image_file_path) + "_intermediate_")
        intermediate_image_file_path = intermediate_image_file_path.with_suffix("." + image_file_format)
        intermediate_image_file_path = str(intermediate_image_file_path.resolve())

        self.renderer.set_num_render_threads(blender.get_render_threads(b_scene))

        host = "127.0.0.1" # the server's hostname or IP address
        port = 7000 # the port used by the server
        self.renderer.set_port(port) 

        # Test if the render is canceled once before running the renderer (export can take a long time
        # and user might want to cancel the rendering during that time)
        if self.test_break():
            print("render canceled")
            return

        self.renderer.run()
        b_render_result = self.begin_result(0, 0, width_px, height_px)

        # Receive and display intermediate render result from the server
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
            # Connect to the rendering server
            # max_retries = 100
            max_retries = 1000
            num_retries = 0
            is_connected = False
            while num_retries < max_retries:
                try:
                    s.settimeout(poll_seconds)
                    s.connect((host, port))
                    is_connected = True
                    print("note: connected to the rendering server")
                    break
                except OSError:
                    print("note: waiting for server to respond... (attempt %d/%d)" % (num_retries + 1, max_retries))
                    num_retries += 1
                    time.sleep(poll_seconds)

                    # User can cancel the render if server failed to respond for too long
                    if self.test_break():
                        print("render canceled")
                        self.renderer.exit()
                        break

            # Connection established
            if is_connected:
                s.setblocking(False)

                num_chunk_bytes = 1 * 1024 * 1024
                data = bytes()
                num_image_bytes = -1

                # Keep receiving data until program terminated or connection ended
                while self.renderer.is_running():
                    try:
                        select_timeout = poll_seconds
                        ready_for_read, ready_for_write, in_error = select.select([s], [], [], select_timeout)
                    except select.error as e:
                        # TODO: maybe reconnect is needed on some error type
                        print("connection to the rendering server ended: %s" % str(e))
                        break

                    # Read incoming data as fast as possible
                    if ready_for_read:
                        data += s.recv(num_chunk_bytes)

                        # Data for the image size received
                        if num_image_bytes < 0 and len(data) >= 8:
                            num_image_bytes = int.from_bytes(data[:8], sys.byteorder)

                        # Data for the image received
                        if num_image_bytes > 0 and len(data) >= 8 + num_image_bytes:
                            with open(intermediate_image_file_path, 'w+b') as image_file:
                                image_file.write(data[8:8 + num_image_bytes])
                                image_file.flush()
                                image_file.seek(0)
                                b_render_result.load_from_file(intermediate_image_file_path)

                            # Signal that pixels have been updated and can be redrawn in the user interface
                            self.update_result(b_render_result)

                            # Chop current image data off
                            data = data[8 + num_image_bytes:]

                            # Reset image size to an unset state
                            num_image_bytes = -1

                    if self.test_break():
                        print("render canceled")
                        self.renderer.exit()
                        break

                # Shutdown sends data and is only allowed when the socket is connected
                # 0 = done receiving, 1 = done sending, 2 = both
                s.shutdown(2)
            else:
                print("warning: connection failed")

        while self.renderer.is_running():
            print("waiting for renderer to finish running...")
            time.sleep(poll_seconds)
        
        self.renderer.exit()

        # Load and display the final image
        is_canceled = self.test_break()
        if not is_canceled:
            image_file_path = image_file_path.with_suffix("." + image_file_format)
            b_render_result.load_from_file(str(image_file_path))
            self.update_result(b_render_result)

        self.end_result(b_render_result)

    # For viewport renders, this method gets called once at the start and whenever the scene or 3D viewport 
    # changes. This method is where data should be read from Blender in the same thread. Typically a render
    # thread will be started to do the work while keeping Blender responsive.
    def view_update(self, b_context, b_depsgraph):
        print("view_update")

    # For viewport renders, this method is called whenever Blender redraws the 3D viewport. The renderer
    # is expected to quickly draw the render with OpenGL, and not perform other expensive work. 
    # Blender will draw overlays for selection and editing on top of the rendered image automatically.
    def view_draw(self, b_context, b_depsgraph):
        print("view_draw")

    @staticmethod
    def _get_temp_folder_path(unique_identifier):
        folder_name = "__photon_temp_" + unique_identifier

        blend_file_folder_path = bpy.path.abspath("//")
        if bpy.data.is_saved and blend_file_folder_path:
            folder_path = Path(blend_file_folder_path)
        else:
            folder_path = Path(tempfile.gettempdir())

        return (folder_path / folder_name).resolve()


class PhRenderPanel(bpy.types.Panel):
    """
    Base type for Photon render panels.
    """

    bl_space_type = "PROPERTIES"
    bl_region_type = "WINDOW"
    bl_context = "render"

    @classmethod
    def poll(cls, context):
        render_settings = context.scene.render
        return render_settings.engine in settings.photon_engines


@blender.register_class
class PH_RENDERING_PT_rendering(PhRenderPanel):
    bl_label = "PR: Rendering"

    bpy.types.Scene.ph_render_integrator_type = bpy.props.EnumProperty(
        items=[
            ('BVPT', "Pure Path Tracing", "Slow but versatile", 0),
            ('BNEEPT', "NEE Path Tracing", "Similar to pure PT but good on rendering small lights", 1),
            ('VPM', "Photon Mapping", "Rough preview, fairly good at caustics", 2),
            ('PPM', "Progressive Photon Mapping", "Good at complex lighting condition", 3),
            ('SPPM', "Stochastic Progressive Photon Mapping", "Good at complex lighting condition", 4),
            ('PPPM', "Probabilistic Progressive Photon Mapping", "Good at complex lighting condition. Consumes more memory but may render in a shorter time than SPPM.", 5),
            ('BVPTDL', "Pure Path Tracing (Direct Lighting)", "", 6),
            ('ATTRIBUTE', "Attribute", "", 7),
            ('CUSTOM', "Custom", "Directly input SDL commands for renderer.", 8)
        ],
        name="Rendering Method",
        description="Photon-v2's rendering methods",
        default='BNEEPT'
    )

    bpy.types.Scene.ph_scheduler_type = bpy.props.EnumProperty(
        items=[
            ('BULK', "Bulk", "", 0),
            ('STRIPE', "Stripe", "", 1),
            ('GRID', "Grid", "", 2),
            ('TILE', "Tile", "", 3),
            ('SPIRAL', "Spiral", "", 4),
            ('SPIRAL_GRID', "Spiral-grid", "", 5)
        ],
        name="Scheduler",
        description="Order of rendering for pixels",
        default='SPIRAL_GRID'
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

    bpy.types.Scene.ph_render_glossy_merge_begin_length = bpy.props.IntProperty(
        name="Glossy Merge Begin Length",
        description="",
        default=1,
        min=1,
        max=16384
    )

    bpy.types.Scene.ph_render_stochastic_view_sample_begin_length = bpy.props.IntProperty(
        name="Stochastic View Sample Begin Length",
        description="",
        default=3,
        min=1,
        max=16384
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

        b_layout.prop(b_scene, 'ph_render_integrator_type')

        render_method = b_scene.ph_render_integrator_type
        if render_method == 'BVPT' or render_method == 'BNEEPT' or render_method == 'BVPTDL' or render_method == 'ATTRIBUTE':
            b_layout.prop(b_scene, 'ph_render_sample_filter_type')
            if render_method != 'ATTRIBUTE':
                b_layout.prop(b_scene, 'ph_scheduler_type')
        elif render_method == 'VPM' or render_method == 'PPM' or render_method == 'SPPM' or render_method == 'PPPM':
            b_layout.prop(b_scene, 'ph_render_num_photons')
            b_layout.prop(b_scene, 'ph_render_num_spp_pm')
            b_layout.prop(b_scene, 'ph_render_num_passes')
            b_layout.prop(b_scene, 'ph_render_kernel_radius')
            b_layout.prop(b_scene, 'ph_render_glossy_merge_begin_length')
            b_layout.prop(b_scene, 'ph_render_stochastic_view_sample_begin_length')
        elif render_method == 'CUSTOM':
            b_layout.prop(b_scene, 'ph_render_custom_sdl')
        else:
            pass

        if render_method != 'CUSTOM':
            b_layout.prop(b_scene, "ph_use_crop_window")

        use_crop_window = b_scene.ph_use_crop_window
        if use_crop_window and render_method != 'CUSTOM':
            b_layout.prop(b_scene, 'ph_crop_min_x')
            b_layout.prop(b_scene, 'ph_crop_min_y')
            b_layout.prop(b_scene, 'ph_crop_width')
            b_layout.prop(b_scene, 'ph_crop_height')


@blender.register_class
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
            ('BOX', "Box", "box filter", 0),
            ('GAUSSIAN', "Gaussian", "Gaussian filter", 1),
            ('MN', "Mitchell-Netravali", "Mitchell-Netravali filter", 2),
            ('BH', "Blackman-Harris", "Blackman-Harris filter", 3)
        ],
        name="Sample Filter",
        description="Photon-v2's sample filter types",
        default='BH'
    )

    bpy.types.Scene.ph_render_sample_source_type = bpy.props.EnumProperty(
        items=[
            ('uniform-random', "Random", "Pseudorandom numbers", 0),
            ('stratified', "Stratified", "Quasirandom numbers based on domain subdivision", 1),
            ('halton', "Halton", "Quasirandom numbers that are well distributed over all dimensions", 2)
        ],
        name="Sample Source",
        description="Photon-v2's sample source types",
        default='stratified'
    )

    bpy.types.Scene.ph_render_halton_permutation = bpy.props.EnumProperty(
        items=[
            ('none', "None", "", 0),
            ('fixed', "Fixed", "", 1),
            ('per-digit', "Per-digit", "", 2),
            ('owen', "Owen", "", 3)
        ],
        name="Halton Permutation",
        description="",
        default='fixed'
    )

    bpy.types.Scene.ph_render_halton_sequence = bpy.props.EnumProperty(
        items=[
            ('original', "Original", "", 0),
            ('leap', "Leap", "", 1),
            ('random-leap', "Random Leap", "", 2),
            ('random-start', "Random Start", "", 3),
        ],
        name="Halton Sequence",
        description="",
        default='original'
    )

    def draw(self, b_context):
        b_scene = b_context.scene
        layout = self.layout

        layout.prop(b_scene, 'ph_render_num_spp')
        layout.prop(b_scene, 'ph_render_sample_filter_type')
        layout.prop(b_scene, 'ph_render_sample_source_type')

        if b_scene.ph_render_sample_source_type == 'halton':
            layout.prop(b_scene, 'ph_render_halton_permutation')
            layout.prop(b_scene, 'ph_render_halton_sequence')


@blender.register_class
class PH_RENDERING_PT_data_structures(PhRenderPanel):
    bl_label = "PR: Data Structures"

    bpy.types.Scene.ph_top_level_accelerator = bpy.props.EnumProperty(
        items=[
            ('BF', "Brute Force", "Iteratively test every object. Super-duper SLOW.", 0),
            ('BVH', "BVH", "Bounding volume hierarchy.", 1),
            ('IKD', "Indexed Kd-tree", "Indexed Kd-tree.", 2)
        ],
        name="Top Level Accelerator",
        description="Type of the first level of acceleration structures.",
        default='BVH'
    )

    def draw(self, b_context):
        b_scene = b_context.scene
        b_layout = self.layout

        b_layout.prop(b_scene, "ph_top_level_accelerator")


@blender.register_class
class PH_RENDERING_PT_performance(PhRenderPanel):
    bl_label = "PR: Performance"

    bpy.types.Scene.ph_num_reserved_threads = bpy.props.IntProperty(
        name="Reserved Threads",
        description="Number of reserved threads for other background tasks.",
        default=0,
        min=0
    )

    def draw(self, b_context):
        b_scene = b_context.scene
        b_layout = self.layout

        b_layout.prop(b_scene.render, 'threads_mode')

        b_sub_col = b_layout.column(align=True)
        b_sub_col.enabled = b_scene.render.threads_mode == 'FIXED'
        b_sub_col.prop(b_scene.render, "threads")

        b_layout.prop(b_scene, "ph_num_reserved_threads")


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


@blender.register_module
class RendererModule(blender.BlenderModule):
    def register(self):
        # RenderEngines also need to tell UI Panels that they are compatible;
        # otherwise most of the UI will be empty when the engine is selected.
        properties_render.RENDER_PT_color_management.COMPAT_ENGINES.add(PhPhotonRenderEngine.bl_idname)
        properties_output.RENDER_PT_format.COMPAT_ENGINES.add(PhPhotonRenderEngine.bl_idname)
        properties_data_camera.DATA_PT_lens.COMPAT_ENGINES.add(PhPhotonRenderEngine.bl_idname)
        properties_data_camera.DATA_PT_camera.COMPAT_ENGINES.add(PhPhotonRenderEngine.bl_idname)
        # properties_data_light.DATA_PT_light.COMPAT_ENGINES.add(PhotonRenderer.bl_idname)
        # properties_data_light.DATA_PT_area.COMPAT_ENGINES.add(PhotonRenderer.bl_idname)
        # properties_material.MATERIAL_PT_preview.COMPAT_ENGINES.add(PhotonRenderer.bl_idname)

    def unregister(self):
        properties_render.RENDER_PT_color_management.COMPAT_ENGINES.remove(PhPhotonRenderEngine.bl_idname)
        properties_output.RENDER_PT_format.COMPAT_ENGINES.remove(PhPhotonRenderEngine.bl_idname)
        properties_data_camera.DATA_PT_lens.COMPAT_ENGINES.remove(PhPhotonRenderEngine.bl_idname)
        properties_data_camera.DATA_PT_camera.COMPAT_ENGINES.remove(PhPhotonRenderEngine.bl_idname)
        # properties_data_light.DATA_PT_light.COMPAT_ENGINES.remove(PhotonRenderer.bl_idname)
        # properties_data_light.DATA_PT_area.COMPAT_ENGINES.remove(PhotonRenderer.bl_idname)
        # properties_material.MATERIAL_PT_preview.COMPAT_ENGINES.remove(PhotonRenderer.bl_idname)
