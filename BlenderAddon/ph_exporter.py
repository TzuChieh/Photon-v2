bl_info = {
    "name"    : "p2 file exporter", 
    "author"  : "Tzu-Chieh Chang", 
    "category": "Photon-v2"
}

import bpy
import math
import mathutils

# ExportHelper is a helper class, defines filename and
# invoke() function which calls the file selector.
from bpy_extras.io_utils import ExportHelper
from bpy.props import StringProperty, BoolProperty, EnumProperty
from bpy.types import Operator

def export_object_mesh(p2File, obj, scene):
	# this creates a temporary mesh data with all modifiers applied for exporting
	# (don't forget to delete it after exporting)
	mesh = obj.to_mesh(scene, True, "RENDER")

	positions = ""
	texcoords = ""
	normals   = ""
	for polygon in mesh.polygons:
		for loopIndex in polygon.loop_indices:
			vertexIndex = mesh.loops[loopIndex].vertex_index
			triVertex = mesh.vertices[vertexIndex]

			triPosition = triVertex.co
			triNormal   = triVertex.normal

			triTexCoord = [0, 0]
			if len(mesh.uv_layers) != 0:
				triTexCoord = mesh.uv_layers[0].data[loopIndex].uv

			# also convert position & normal to Photon-v2's coordinate system
			positions += "\"%.8f %.8f %.8f\" " %(triPosition.y, triPosition.z, triPosition.x)
			texcoords += "\"%.8f %.8f %.8f\" " %(triTexCoord[0], triTexCoord[1], 0)
			normals   += "\"%.8f %.8f %.8f\" " %(triNormal.y, triNormal.z, triNormal.x)

	# delete the temporary mesh for exporting
	bpy.data.meshes.remove(mesh)

	geometryName = "@geometry_" + obj.name + "_" + obj.data.name

	p2File.write("-> geometry %s [string type triangle-mesh]\n" %(geometryName))
	p2File.write("[vector3r-array positions {%s}]\n"           %(positions))
	p2File.write("[vector3r-array texture-coordinates {%s}]\n" %(texcoords))
	p2File.write("[vector3r-array normals {%s}]\n"             %(normals))

	return geometryName

def export_object_material(p2File, material, scene):
	materialName = "@material_" + material.name

	albedo = material.ph_albedo
	p2File.write("-> material %s [string type matte-opaque] [vector3r albedo \"%.8f %.8f %.8f\"]\n" 
	             %(materialName, albedo[0], albedo[1], albedo[2]))

	return materialName

def export_actor_model(p2File, obj, scene):
	if obj.active_material != None:
		geometryName   = export_object_mesh(p2File, obj, scene)
		materialName   = export_object_material(p2File, obj.active_material, scene)
		actorModelName = "@actor_model_" + obj.name

		# creating actor-model, also convert transformation to Photon-v2's coordinate system
		pos, rot, scale = obj.matrix_world.decompose()
		p2File.write("-> actor-model %s [geometry geometry %s] [material material %s]\n" %(actorModelName, geometryName, materialName))
		p2File.write("-> transform [string type translate] [actor-model target %s] [vector3r factor \"%.8f %.8f %.8f\"]\n" 
		             %(actorModelName, pos.y, pos.z, pos.x))
		p2File.write("-> transform [string type scale] [actor-model target %s] [vector3r factor \"%.8f %.8f %.8f\"]\n"
		             %(actorModelName, scale.y, scale.z, scale.x))
		p2File.write("-> transform [string type rotate] [actor-model target %s] [quaternionR factor \"%.8f %.8f %.8f %.8f\"]\n"
		             %(actorModelName, rot.y, rot.z, rot.x, rot.w))
	else:
		print("warning: mesh<%s> has no material, not exporting" %(obj.name))

def export_actor_light(p2File, obj, scene):
	lamp = obj.data

	if lamp.type == "AREA":
		lightMaterialName = "@material_" + obj.name + "_" + lamp.name
		lightGeometryName = "@geometry_" + obj.name + "_" + lamp.name
		lightSourceName   = "@light_source_" + obj.name + "_" + lamp.name
		actorLightName    = "@actor_light_" + obj.name

		recWidth  = lamp.size
		recHeight = lamp.size
		if lamp.shape == "RECTANGLE":
			recHeight = lamp.size_y

		p2File.write("-> geometry %s [string type rectangle] [real width %.8f] [real height %.8f]\n" 
		             %(lightGeometryName, recWidth, recHeight))
		p2File.write("-> material %s [string type matte-opaque] [vector3r albedo \"0.5 0.5 0.5\"]\n" 
	                 %(lightMaterialName))

		# use lamp's color attribute as emitted radiance
		emittedRadiance = lamp.color

		p2File.write("-> light-source %s [string type area] [vector3r emitted-radiance \"%.8f %.8f %.8f\"]\n" 
		             %(lightSourceName, emittedRadiance[0], emittedRadiance[1], emittedRadiance[2]))

		# creating actor-light, also convert transformation to Photon-v2's coordinate system

		pos, rot, scale = obj.matrix_world.decompose()

		p2File.write("-> actor-light %s [light-source light-source %s] [geometry geometry %s] [material material %s]\n" 
		             %(actorLightName, lightSourceName, lightGeometryName, lightMaterialName))
		p2File.write("-> transform [string type translate] [actor-light target %s] [vector3r factor \"%.8f %.8f %.8f\"]\n" 
		             %(actorLightName, pos.y, pos.z, pos.x))
		p2File.write("-> transform [string type scale] [actor-light target %s] [vector3r factor \"%.8f %.8f %.8f\"]\n"
		             %(actorLightName, scale.y, scale.z, scale.x))

		# Blender's rectangle is defined in xy-plane, which is Photon-v2's xz-plane, this rotation accounts for that
		p2File.write("-> transform [string type rotate] [actor-light target %s] [vector3r axis \"1 0 0\"] [real degree -90]\n"
			         %(actorLightName))

		p2File.write("-> transform [string type rotate] [actor-light target %s] [quaternionR factor \"%.8f %.8f %.8f %.8f\"]\n"
		             %(actorLightName, rot.y, rot.z, rot.x, rot.w))

	else:
		print("warning: lamp (%s) type (%s) is unsupported, not exporting" %(lamp.name, lamp.type))

def export_camera(p2File, obj, scene):
	camera = obj.data
	if camera.type == "PERSP":
		if camera.lens_unit == "FOV":
			fovDegrees = math.degrees(camera.angle)

			# creating camera, also convert transformation to Photon-v2's coordinate system
			# (Blender's camera intially pointing (0, 0, -1) with up (0, 1, 0) in its coordinate system.)

			pos, rot, scale = obj.matrix_world.decompose()

			if (scale.x - 1.0) > 0.0001 or (scale.y - 1.0) > 0.0001 or (scale.z - 1.0) > 0.0001:
				print("warning: camera (%s) contains scale factor, ignoring" %(camera.name))

			camDir   = rot * mathutils.Vector((0, 0, -1))
			camUpDir = rot * mathutils.Vector((0, 1, 0))

			p2File.write("""## camera [string type pinhole] [real fov-degree %.8f] 
			             [vector3r position \"%.8f %.8f %.8f\"] [vector3r direction \"%.8f %.8f %.8f\"] [vector3r up-axis \"%.8f %.8f %.8f\"]\n"""
			             %(fovDegrees, pos.y, pos.z, pos.x, camDir.y, camDir.z, camDir.x, camUpDir.y, camUpDir.z, camUpDir.x))
		else:
			print("warning: camera (%s) with lens unit %s is unsupported, not exporting" %(camera.name, camera.lens_unit))
	else:
		print("warning: camera (%s) type (%s) is unsupported, not exporting" %(camera.name, camera.type))

def export_core_commands(p2File, scene):
	objs = scene.objects
	for obj in objs:
		if obj.type == "CAMERA":
			export_camera(p2File, obj, scene)

	p2File.write("## film [integer width 400] [integer height 400] \n")
	p2File.write("## sampler [integer spp-budget 16] \n")
	p2File.write("## integrator [string type backward-mis] \n")

def export_world_commands(p2File, scene):
	objs = scene.objects
	for obj in objs:
		if obj.type == "MESH":
			export_actor_model(p2File, obj, scene)
		elif obj.type == "LAMP":
			export_actor_light(p2File, obj, scene)
		elif obj.type == "CAMERA":
			# do nothing since it belongs to core command
			return
		else:
			print("warning: object (%s) type (%s) is not supported, not exporting" %(obj.name, obj.type))

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
		print("exporting p2 file to <%s>" %(self.filepath))

		p2File = open(self.filepath, "w", encoding = "utf-8")
		scene = bpy.context.scene
		export_core_commands(p2File, scene)
		export_world_commands(p2File, scene)
		p2File.close()

		print("exporting complete")
		return {"FINISHED"}

# Only needed if you want to add into a dynamic menu
def menu_func_export(self, context):
	self.layout.operator(P2Exporter.bl_idname, text = "P2 (.p2)")

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