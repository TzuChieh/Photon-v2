from .export import Exporter
from .material import helper
from utility import blender, settings
from .mesh import helper
from . import scene
import pres

import bpy
import bpy_extras
from bl_ui import properties_output, properties_data_camera

from pathlib import Path


class ExporterCache:
    def __init__(self):
        super().__init__()
        self.subdiv_original_settings = None
        self.autosmooth_original_settings = None


def save_scene(scene_path, scene_name, b_depsgraph: bpy.types.Depsgraph):
    b_scene = b_depsgraph.scene_eval

    exporter = Exporter(scene_path)
    exporter.begin(scene_name)
    exporter.export_core_commands(b_scene)
    exporter.export(b_depsgraph)
    exporter.export_options(b_scene)
    exporter.end()


# ExportHelper is a helper class, defines filename and invoke() function which calls the file selector.
class OBJECT_OT_p2_exporter(bpy.types.Operator, bpy_extras.io_utils.ExportHelper):
    """
    Export the scene to a format that is readable by Photon-v2.
    """

    bl_idname = "object.p2_exporter"
    bl_label = "Photon SDL"

    # ExportHelper mixin class uses this
    filename_ext = ""

    # filter_glob = StringProperty(
    # 	default="*.p2",
    # 	options={"HIDDEN"},
    # )

    is_animation: bpy.props.BoolProperty(
        name="Export Animation",
        description="Export each frame as a separate scene file.",
        default=False
    )

    # TODO: able to force specific level

    subdivision_quality: bpy.props.EnumProperty(
        items=[
            ('VIEWPORT', "Viewport", "The level as seen in the viewport.", 0),
            ('RENDER', "Render", "Final render quality.", 1),
        ],
        name="Subdivision Quality",
        description="The subdivision quality of exported mesh.",
        default='RENDER'
    )

    def execute(self, b_context: bpy.types.Context):
        # Blender may not write data while editing--we want to avoid exporting in edit mode so data will 
        # be complete
        edit_modes = {
            'EDIT_MESH',
            'EDIT_CURVE',
            'EDIT_SURFACE',
            'EDIT_TEXT',
            'EDIT_ARMATURE',
            'EDIT_METABALL',
            'EDIT_LATTICE',
            'EDIT_GPENCIL'
        }
        if b_context.mode in edit_modes:
            print("Export failed. Please exit edit mode for exporting.")
            return {'CANCELLED'}

        # Make sure we are getting up-to-date data before obtaining depsgraph
        b_context.view_layer.update()

        if not self.is_animation:
            cache = ExporterCache()
            b_depsgraph = self.get_evaluated_depsgraph(b_context, cache)
            save_scene(self.filepath, "scene", b_depsgraph)
            self.restore_modified_settings(b_context, cache)
        else:
            b_scene = b_context.scene
            for frame_number in range(b_scene.frame_start, b_scene.frame_end + 1):
                print("Exporting frame", frame_number)
                b_scene.frame_set(frame_number)

                cache = ExporterCache()
                b_depsgraph = self.get_evaluated_depsgraph(b_context, cache)
                save_scene(self.filepath, "scene_" + str(frame_number).zfill(6), b_depsgraph)
                self.restore_modified_settings(b_context, cache)

        return {'FINISHED'}


    def get_evaluated_depsgraph(self, b_context: bpy.types.Context, cache: ExporterCache):
        """
        Replacement for `bpy.types.Context.evaluated_depsgraph_get()`. Unfortunately Blender currently does
        not support getting a depsgraph with `depsgraph.mode == 'RENDER'` (except the one passed to
        `bpy.types.RenderEngine` by Blender). This will cause the exporter to export in `VIEWPORT` mode,
        and many settings such as subdivision modifiers will not apply their `RENDER` mode settings.
        This method attempts to enhance this part by trying to automatically adjust object settings to
        match the one used for final rendering (or what the user specifies).
        """
        b_depsgraph = b_context.evaluated_depsgraph_get()

        b_mesh_objects = scene.find_mesh_objects(b_depsgraph)

        # Force subdivision level if required
        should_force_subdiv = b_depsgraph.mode != self.subdivision_quality
        subdiv_original_settings = {}
        if should_force_subdiv:
            for b_evaluated_mesh_object in b_mesh_objects:
                b_mesh_object = b_evaluated_mesh_object.original
                helper.mesh_object_force_subdiv_level(
                    b_mesh_object,
                    self.subdivision_quality,
                    subdiv_original_settings)
        
        cache.subdiv_original_settings = subdiv_original_settings

        # Emulate autosmooth settings with edge split modifier
        autosmooth_original_settings = {}
        for b_evaluated_mesh_object in b_mesh_objects:
            b_mesh_object = b_evaluated_mesh_object.original
            helper.mesh_object_autosmooth_to_edgesplit(
                b_mesh_object,
                autosmooth_original_settings)

        cache.autosmooth_original_settings = autosmooth_original_settings

        # Re-evaluate any modified data-blocks, for example for animation or modifiers. 
        # This invalidates all references to evaluated data-blocks from this dependency graph.
        b_depsgraph.update()
        return b_depsgraph

    def restore_modified_settings(self, b_context: bpy.types.Context, cache: ExporterCache):
        """
        Calling `get_evaluated_depsgraph()` may modify scene settings. This method restores the settings
        modified to their original states.
        """
        b_depsgraph = b_context.evaluated_depsgraph_get()

        # Restore mesh objects to original settings

        b_mesh_objects = scene.find_mesh_objects(b_depsgraph)

        if cache.subdiv_original_settings is not None:
            for b_evaluated_mesh_object in b_mesh_objects:
                b_mesh_object = b_evaluated_mesh_object.original
                helper.restore_mesh_object_subdiv_level(b_mesh_object, cache.subdiv_original_settings)

        if cache.autosmooth_original_settings is not None:
            for b_evaluated_mesh_object in b_mesh_objects:
                b_mesh_object = b_evaluated_mesh_object.original
                helper.restore_mesh_object_autosmooth(b_mesh_object, cache.autosmooth_original_settings)


# Add exporter into a dynamic menu
def menu_func_export(self, b_context):
    self.layout.operator(OBJECT_OT_p2_exporter.bl_idname, text="Photon Scene (.p2)")


@blender.register_class
class PhPhotonExportEngine(bpy.types.RenderEngine):
    # These three members are used by blender to set up the
    # RenderEngine; define its internal name, visible name and capabilities.
    bl_idname = settings.exporter_engine_id_name
    bl_label = "Render to .p2"
    bl_use_preview = False

    # Do not expose Cycles and Eevee shading nodes in the node editor user interface, so own nodes can be used instead.
    bl_use_shading_nodes_custom = True

    # No need to save image for animation
    bl_use_image_save = False

    # This is required to disable image saving
    bl_use_postprocess = False

    # Init is called whenever a new render engine instance is created. Multiple instances may exist at the same 
    # time, for example for a viewport and final render.
    def __init__(self):
        super().__init__()

        print("Photon render-to-p2 engine started")

    # When the render engine instance is destroyed, this is called. Clean up any render engine data here, 
    # for example stopping running render threads.
    def __del__(self):
        print("Photon render-to-p2 engine exited")

    # Export scene data for render
    def update(self, b_blend_data, b_depsgraph):
        pass

    # This is the method called by Blender for both final renders (F12) and small preview for materials, world
    # and lights.
    def render(self, b_depsgraph):
        self._report_info("Start exporting scene file(s).")

        b_scene = b_depsgraph.scene_eval
        if not b_scene.ph_export_name:
            self._report_error("Scene must have a name.")
            return

        scene_dir = bpy.path.abspath(b_scene.ph_export_directory)
        scene_path = Path(scene_dir) / b_scene.ph_export_name

        if not self.is_animation:
            self._report_info("Exporting single frame... (open console to track its progress)")

            save_scene(scene_path, "scene", b_depsgraph)
        else:
            frame_number = b_scene.frame_current
            self._report_info("Exporting frame %d... (open console to track its progress)" % frame_number)

            save_scene(scene_path, "scene_" + str(frame_number).zfill(6), b_depsgraph)

        self._report_info("Finished exporting scene file(s).")

        # TODO: ability to cancel

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

    def _report_info(self, msg):
        self.report({'INFO'}, msg)

    def _report_error(self, msg):
        self.report({'ERROR'}, msg)


@blender.register_class
class PH_RENDERING_PT_exporting(bpy.types.Panel):
    bl_space_type = 'PROPERTIES'
    bl_region_type = 'WINDOW'
    bl_context = "render"
    bl_label = "PR: Exporting"

    bpy.types.Scene.ph_export_directory = bpy.props.StringProperty(
		name="Directory",
		default="//",
		subtype='DIR_PATH'
	)

    bpy.types.Scene.ph_export_name = bpy.props.StringProperty(
		name="Name",
		default="scene_export"
	)

    @classmethod
    def poll(cls, context):
        render_settings = context.scene.render
        return render_settings.engine == settings.exporter_engine_id_name

    def draw(self, b_context):
        b_scene = b_context.scene
        b_layout = self.layout

        b_layout.prop(b_scene, 'ph_export_directory')
        b_layout.prop(b_scene, 'ph_export_name')


@blender.register_module
class ExporterModule(blender.BlenderModule):
    def register(self):
        bpy.utils.register_class(OBJECT_OT_p2_exporter)
        bpy.types.TOPBAR_MT_file_export.append(menu_func_export)

        # RenderEngines also need to tell UI Panels that they are compatible;
        # otherwise most of the UI will be empty when the engine is selected.
        properties_output.RENDER_PT_format.COMPAT_ENGINES.add(PhPhotonExportEngine.bl_idname)
        properties_data_camera.DATA_PT_lens.COMPAT_ENGINES.add(PhPhotonExportEngine.bl_idname)
        properties_data_camera.DATA_PT_camera.COMPAT_ENGINES.add(PhPhotonExportEngine.bl_idname)

    def unregister(self):
        bpy.types.TOPBAR_MT_file_export.remove(menu_func_export)
        bpy.utils.unregister_class(OBJECT_OT_p2_exporter)

        properties_output.RENDER_PT_format.COMPAT_ENGINES.remove(PhPhotonExportEngine.bl_idname)
        properties_data_camera.DATA_PT_lens.COMPAT_ENGINES.remove(PhPhotonExportEngine.bl_idname)
        properties_data_camera.DATA_PT_camera.COMPAT_ENGINES.remove(PhPhotonExportEngine.bl_idname)
