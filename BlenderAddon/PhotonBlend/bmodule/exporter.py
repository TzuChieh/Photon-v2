from .export import Exporter
from .material import helper
from ..utility import blender
from .mesh import helper
from . import scene

import bpy
import bpy_extras


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
            ("VIEWPORT", "Viewport", "The level as seen in the viewport."),
            ("RENDER", "Render", "Final render quality."),
        ],
        name="Subdivision Quality",
        description="The subdivision quality of exported mesh.",
        default="RENDER"
    )

    def execute(self, b_context):
        # Blender may not write data while editing--we want to avoid exporting in edit mode so data will be complete
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

        b_scene = b_context.scene
        if not self.is_animation:
            self.save_scene("scene", b_scene, self.get_evaluated_depsgraph(b_context))
        else:
            for frame_number in range(b_scene.frame_start, b_scene.frame_end + 1):
                print("Exporting frame", frame_number)
                b_scene.frame_set(frame_number)
                self.save_scene("scene_" + str(frame_number).zfill(6), b_scene, self.get_evaluated_depsgraph(b_context))

        return {'FINISHED'}

    def save_scene(self, scene_name, b_scene, b_depsgraph: bpy.types.Depsgraph):
        exporter = Exporter(self.filepath)
        exporter.begin(scene_name)
        exporter.export_core_commands(b_scene)
        exporter.export(b_depsgraph)
        exporter.end()

    def get_evaluated_depsgraph(self, b_context):
        # Make sure we are getting up-to-date data before obtaining depsgraph
        b_context.view_layer.update()
        b_depsgraph = b_context.evaluated_depsgraph_get()

        # Force subdivision level if required
        if b_depsgraph.mode != self.subdivision_quality:
            original_settings = {}
            b_mesh_objects = scene.find_mesh_objects(b_depsgraph)

            for b_evaluated_mesh_object in b_mesh_objects:
                b_mesh_object = b_evaluated_mesh_object.original
                helper.force_mesh_object_subdiv_level(b_mesh_object, original_settings, level=self.subdivision_quality)

            # Make sure we are getting up-to-date data before updating depsgraph
            b_context.view_layer.update()
            b_depsgraph.update()

            # After getting the forced-level depsgraph, restore mesh objects to original settings
            for b_evaluated_mesh_object in b_mesh_objects:
                b_mesh_object = b_evaluated_mesh_object.original
                helper.restore_mesh_object_subdiv_level(b_mesh_object, original_settings)

        return b_depsgraph


# Add exporter into a dynamic menu
def menu_func_export(self, b_context):
    self.layout.operator(OBJECT_OT_p2_exporter.bl_idname, text="Photon Scene (.p2)")


class ExporterModule(blender.BlenderModule):
    def register(self):
        bpy.utils.register_class(OBJECT_OT_p2_exporter)
        bpy.types.TOPBAR_MT_file_export.append(menu_func_export)

    def unregister(self):
        bpy.types.TOPBAR_MT_file_export.remove(menu_func_export)
        bpy.utils.unregister_class(OBJECT_OT_p2_exporter)


def include_module(module_manager):
    module_manager.add_module(ExporterModule())
