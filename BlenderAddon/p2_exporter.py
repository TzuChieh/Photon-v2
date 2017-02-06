bl_info = {
    "name"    : "p2 file exporter", 
    "author"  : "Tzu-Chieh Chang", 
    "category": "Photon-v2"
}

import bpy

# ExportHelper is a helper class, defines filename and
# invoke() function which calls the file selector.
from bpy_extras.io_utils import ExportHelper
from bpy.props import StringProperty, BoolProperty, EnumProperty
from bpy.types import Operator

def export_core_commands(p2File, context):
    p2File.write("core commands... \n")
    
def export_world_commands(p2File, context):
    p2File.write("world commands... \n")

class P2Exporter(Operator, ExportHelper):
    """export the scene to some Photon-v2 readable format"""
    bl_idname = "object.p2_exporter"
    bl_label  = "export p2"

    # ExportHelper mixin class uses this
    filename_ext = ".p2"

    filter_glob = StringProperty(
            default="*.p2",
            options={"HIDDEN"},
            )

    # List of operator properties, the attributes will be assigned
    # to the class instance from the operator settings before calling.
    use_setting = BoolProperty(
            name="Example Boolean",
            description="Example Tooltip",
            default=True,
            )

    type = EnumProperty(
            name="Example Enum",
            description="Choose between two items",
            items=(('OPT_A', "First Option", "Description one"),
                   ('OPT_B', "Second Option", "Description two")),
            default='OPT_A',
            )

    def execute(self, context):
        p2File = open(self.filepath, "w", encoding = "utf-8")

        export_core_commands(p2File, context)
        export_world_commands(p2File, context)

        p2File.close()
        return {"FINISHED"}

# Only needed if you want to add into a dynamic menu
def menu_func_export(self, context):
    self.layout.operator(P2Exporter.bl_idname, text="Text Export Operator")

def register():
    bpy.utils.register_class(P2Exporter)
    bpy.types.INFO_MT_file_export.append(menu_func_export)

def unregister():
    bpy.utils.unregister_class(P2Exporter)
    bpy.types.INFO_MT_file_export.remove(menu_func_export)

if __name__ == "__main__":
    register()

    # test call
    bpy.ops.object.p2_exporter("INVOKE_DEFAULT")