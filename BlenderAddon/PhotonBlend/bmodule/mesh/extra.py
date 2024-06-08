"""
@brief Building extra mesh object types.
"""
from utility import blender

import bpy
import bpy_extras


@blender.register_class
class PH_MESH_OT_add_menger_sponge(bpy.types.Operator, bpy_extras.object_utils.AddObjectHelper):
    bl_idname = 'photon.add_menger_sponge'
    bl_label = "Add Menger Sponge"
    bl_description = "Add a menger sponge (represented by a cuboid)."
    bl_options = {'REGISTER', 'UNDO', 'PRESET'}

    num_iterations: bpy.props.IntProperty(
        name="Iterations",
        description="Number of recursive iterations on generating the sponge.",
        min=0,
        default=3
        )

    def draw(self, b_context):
        b_layout = self.layout

        b_layout.prop(self, 'num_iterations')

    def execute(self, b_context):
        bpy.ops.mesh.primitive_cube_add(size=1.0)
        b_mesh = b_context.active_object.data
        b_mesh.photon.export_type = 'MENGER_SPONGE'
        
        # Store specific attributes as custom properties
        b_mesh['ph_num_iterations'] = self.num_iterations

        return {'FINISHED'}
