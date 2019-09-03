from ..utility import (
        settings,
        blender)

import bpy
from bl_ui import (
        properties_output,
        properties_data_camera)


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
        print("init")

    # When the render engine instance is destroy, this is called. Clean up any render engine data here, for example
    # stopping running render threads.
    def __del__(self):
        print("del")

    # This is the method called by Blender for both final renders (F12) and small preview for materials, world
    # and lights.
    def render(self, b_depsgraph):
        print("render")

        b_scene = b_depsgraph.scene
        width_px = blender.get_render_width_px(b_scene)
        height_px = blender.get_render_height_px(b_scene)

        b_render_result = self.begin_result(0, 0, width_px, height_px)
        b_render_layer = b_render_result.layers[0]
        b_render_layer.load_from_file("C:\\test.exr")

        self.end_result(b_render_result)

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
            ("BVPT", "Pure Path Tracing", "slow but versatile"),
            ("BNEEPT", "NEE Path Tracing", "similar to pure PT but good on rendering small lights"),
            ("VPM", "Photon Mapping", "rough preview, fairly good at caustics"),
            ("PPM", "Progressive Photon Mapping", "good at complex lighting condition"),
            ("SPPM", "Stochastic Progressive Photon Mapping", "good at complex lighting condition"),
            ("BVPTDL", "Pure Path Tracing (Direct Lighting)", ""),
            ("ATTRIBUTE", "Attribute", ""),
            ("CUSTOM", "Custom", "directly input SDL commands for renderer.")
        ],
        name="Rendering Method",
        description="Photon-v2's rendering methods",
        default="BNEEPT"
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
        layout = self.layout

        layout.prop(b_scene, "ph_render_integrator_type")

        render_method = b_scene.ph_render_integrator_type
        if render_method == "BVPT" or render_method == "BNEEPT" or render_method == "BVPTDL" or render_method == "ATTRIBUTE":
            layout.prop(b_scene, "ph_render_num_spp")
            layout.prop(b_scene, "ph_render_sample_filter_type")
        elif render_method == "VPM" or render_method == "PPM" or render_method == "SPPM":
            layout.prop(b_scene, "ph_render_num_photons")
            layout.prop(b_scene, "ph_render_num_spp_pm")
            layout.prop(b_scene, "ph_render_num_passes")
            layout.prop(b_scene, "ph_render_kernel_radius")
        elif render_method == "CUSTOM":
            layout.prop(b_scene, "ph_render_custom_sdl")
        else:
            pass

        if render_method != "CUSTOM":
            layout.prop(b_scene, "ph_use_crop_window")

        use_crop_window = b_scene.ph_use_crop_window
        if use_crop_window and render_method != "CUSTOM":
            layout.prop(b_scene, "ph_crop_min_x")
            layout.prop(b_scene, "ph_crop_min_y")
            layout.prop(b_scene, "ph_crop_width")
            layout.prop(b_scene, "ph_crop_height")


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
        name="Sample Filter Type",
        description="Photon-v2's sample filter types",
        default="BH"
    )

    def draw(self, b_context):
        b_scene = b_context.scene
        layout = self.layout

        layout.prop(b_scene, "ph_render_num_spp")
        layout.prop(b_scene, "ph_render_sample_filter_type")


class PH_RENDERING_PT_options(PhRenderPanel):
    bl_label = "PR: Options"

    bpy.types.Scene.ph_use_cycles_material = bpy.props.BoolProperty(
        name="Use Cycles Material",
        description="render/export the scene with materials converted from Cycles to Photon",
        default=False
    )

    def draw(self, context):
        scene = context.scene
        layout = self.layout

        layout.prop(scene, "ph_use_cycles_material")


RENDER_PANEL_CLASSES = [
    PH_RENDERING_PT_sampling,
    PH_RENDERING_PT_options,
    PH_RENDERING_PT_rendering
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

        properties_output.RENDER_PT_render.COMPAT_ENGINES.remove(PhPhotonRenderEngine.bl_idname)

        properties_data_camera.DATA_PT_lens.COMPAT_ENGINES.remove(PhPhotonRenderEngine.bl_idname)
        properties_data_camera.DATA_PT_camera.COMPAT_ENGINES.remove(PhPhotonRenderEngine.bl_idname)

        # properties_data_light.DATA_PT_light.COMPAT_ENGINES.remove(PhotonRenderer.bl_idname)
        # properties_data_light.DATA_PT_area.COMPAT_ENGINES.remove(PhotonRenderer.bl_idname)

        # properties_material.MATERIAL_PT_preview.COMPAT_ENGINES.remove(PhotonRenderer.bl_idname)

        for clazz in RENDER_PANEL_CLASSES:
            bpy.utils.unregister_class(clazz)


def include_module(module_manager):
    module_manager.add_module(RendererModule())
