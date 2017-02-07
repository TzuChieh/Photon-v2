bl_info = {
    "name"    : "Photon-v2 material", 
    "author"  : "Tzu-Chieh Chang", 
    "category": "Photon-v2"
}

import bpy

class PhMaterialPanel(bpy.types.Panel):
	"""Photon's material panel"""
	bl_label       = "Photon-v2 Material"
	bl_idname      = "MATERIAL_PT_photon"   # "<CATEGORY>_PT_<name>" is a naming convention for panels.
	bl_space_type  = "PROPERTIES"
	bl_region_type = "WINDOW"
	bl_context     = "material"

	bpy.types.Material.ph_albedo = bpy.props.FloatVectorProperty(
	    name    = "albedo",
	    default = [0.5, 0.5, 0.5],
	    subtype = "COLOR",
	    size    = 3
	)

	def draw(self, context):
		layout = self.layout
		obj    = context.object

		layout.prop(obj.active_material, "ph_albedo")

def register():
	bpy.utils.register_class(PhMaterialPanel)

def unregister():
	bpy.utils.unregister_class(PhMaterialPanel)

if __name__ == "__main__":
	register()