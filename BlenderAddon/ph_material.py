bl_info = {
    "name"    : "Photon-v2 material", 
    "author"  : "Tzu-Chieh Chang", 
    "category": "Photon-v2"
}

import bpy
import sys

class PhMaterialPanel(bpy.types.Panel):
	"""Photon's material panel"""
	bl_label       = "Photon-v2 Material"
	bl_idname      = "MATERIAL_PT_photon"   # "<CATEGORY>_PT_<name>" is a naming convention for panels.
	bl_space_type  = "PROPERTIES"
	bl_region_type = "WINDOW"
	bl_context     = "material"

	bpy.types.Material.ph_material_type = bpy.props.EnumProperty(
		items = [
			("MATTE_OPAQUE",        "Matte Opaque",        "diffuse material"), 
			("ABRADED_OPAQUE",      "Abraded Opaque",      "microfacet material"), 
			("ABRADED_TRANSLUCENT", "Abraded Translucent", "microfacet dielectric material")
		],
		name        = "Material Type",
		description = "Photon-v2's material types", 
		default     = "MATTE_OPAQUE"
	)

	bpy.types.Material.ph_albedo = bpy.props.FloatVectorProperty(
	    name        = "albedo", 
	    description = "surface albedo in [0, 1]", 
	    default     = [0.5, 0.5, 0.5], 
	    min         = 0.0, 
		max         = 1.0, 
	    subtype     = "COLOR", 
	    size        = 3
	)

	bpy.types.Material.ph_roughness = bpy.props.FloatProperty(
		name        = "roughness", 
		description = "surface roughness in [0, 1]", 
		default     = 0.5, 
		min         = 0.0, 
		max         = 1.0
	)

	bpy.types.Material.ph_ior = bpy.props.FloatProperty(
		name        = "index of refraction", 
		description = "index of refraction of the material in [0, infinity]", 
		default     = 1.5, 
		min         = 0.0, 
		max         = sys.float_info.max
	)

	bpy.types.Material.ph_f0 = bpy.props.FloatVectorProperty(
		name        = "F0", 
		description = "surface reflectivity at normal incidence in [0, 1]", 
		default     = [0.04, 0.04, 0.04], 
		min         = 0.0, 
		max         = 1.0, 
		subtype     = "COLOR", 
		size        = 3
	)

	def draw(self, context):
		material = context.material
		layout   = self.layout

		if material == None:
			layout.label(material, "no material for Photon-v2 material panel")
			return

		layout.prop(material, "ph_material_type")
		materialType = material.ph_material_type

		if materialType == "MATTE_OPAQUE":

			layout.prop(material, "ph_albedo")

		elif materialType == "ABRADED_OPAQUE":

			layout.prop(material, "ph_albedo")
			layout.prop(material, "ph_f0")
			layout.prop(material, "ph_roughness")

		elif materialType == "ABRADED_TRANSLUCENT":

			layout.prop(material, "ph_albedo")
			layout.prop(material, "ph_f0")
			layout.prop(material, "ph_roughness")
			layout.prop(material, "ph_ior")

		else:
			print("warning: unknown type of Photon-v2 material (%s)" %(materialType))

def register():
	bpy.utils.register_class(PhMaterialPanel)

def unregister():
	bpy.utils.unregister_class(PhMaterialPanel)

if __name__ == "__main__":
	register()