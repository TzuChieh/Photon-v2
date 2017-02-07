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

def export_object_mesh(p2File, obj):
	mesh = obj.data

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

	geometryName   = "@geometry_" + obj.name + "_" + mesh.name
	materialName   = "@material_" + obj.name
	actorModelName = "@actor_model_" + obj.name

	p2File.write("-> geometry %s [string type triangle-mesh]\n" %(geometryName))
	p2File.write("[vector3r-array positions {%s}]\n"           %(positions))
	p2File.write("[vector3r-array texture-coordinates {%s}]\n" %(texcoords))
	p2File.write("[vector3r-array normals {%s}]\n"             %(normals))

	p2File.write("-> material %s [string type matte-opaque] [vector3r albedo \"0.5 0.5 0.5\"]\n" %(materialName))
	p2File.write("-> actor-model %s [geometry geometry %s] [material material %s]\n" %(actorModelName, geometryName, materialName))

def export_object(p2File, obj):
	if obj.type == "MESH":
		export_object_mesh(p2File, obj)
	else:
		print("warning: unsupported object type <%s>" %(obj.name))

def export_core_commands(p2File):
	p2File.write("## camera [string type pinhole] [real fov-degree 50] [vector3r position \"0 0 16\"] [vector3r direction \"0 0 -1\"] \n")
	p2File.write("## film [integer width 400] [integer height 400] \n")
	p2File.write("## sampler [integer spp-budget 16] \n")
	p2File.write("## integrator [string type backward-mis] \n")

def export_world_commands(p2File):
	scene = bpy.context.scene
	objs = scene.objects
	for obj in objs:
		export_object(p2File, obj)

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

		export_core_commands(p2File)
		export_world_commands(p2File)

		p2File.close()
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