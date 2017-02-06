bl_info = {
    "name": "Move X Axis", 
    "category": "Object"
}

import bpy

class ObjectMoveX(bpy.types.Operator):
    """
    multi-line docstring, blender will use this as tooltip...
    """
    bl_idname = "object.move_x"      # unique identifier for buttons and menu items to reference
    bl_label = "move X by one changed"       # display name in the interface
    bl_options = {"REGISTER", "UNDO"}# enable undo for the operator
    
    # execute() is called by blender when running the operator
    def execute(self, context):
        # script goes here
        scene = context.scene
        for obj in scene.objects:
            obj.location.x += 1.0
            
        # this lets blender know the operator finished successfully
        return {"FINISHED"}
    
def register():
    bpy.utils.register_class(ObjectMoveX)
    
def unregister():
    bpy.utils.unregister_class(ObjectMoveX)
    
# This allows you to run the script directly from blenders text editor
# to test the addon without having to install it.
if __name__ == "__main__":
    register()