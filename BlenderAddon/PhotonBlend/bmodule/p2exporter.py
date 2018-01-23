from ..psdl import clause
from ..psdl.sdlconsole import SdlConsole
from ..psdl.cmd import RawCommand
from .. import utility
from ..utility import meta
from . import ui
from . import export

import bpy
import mathutils

# ExportHelper is a helper class, defines filename and
# invoke() function which calls the file selector.
from bpy_extras.io_utils import ExportHelper
from bpy.props import StringProperty, BoolProperty, EnumProperty
from bpy.types import Operator

import math


def mangled_geometry_name(obj, name, suffix):
	return "geometry_" + obj.name + "_" + name + "_" + suffix


def mangled_material_name(obj, name, suffix):
	return "material_" + obj.name + "_" + name + "_" + suffix


def mangled_light_source_name(obj, name, suffix):
	return "light_source_" + obj.name + "_" + name + "_" + suffix


def mangled_actor_model_name(obj, name, suffix):
	return "actor_model_" + obj.name + "_" + name + "_" + suffix


def mangled_actor_light_name(obj, name, suffix):
	return "actor_light_" + obj.name + "_" + name + "_" + suffix


class Exporter:

	def __init__(self, file_path):
		self.__file_path  = file_path
		self.__sdlconsole = None

	def begin(self):

		file_path            = self.__file_path
		folder_path          = utility.get_folder_path(file_path)
		filename_without_ext = utility.get_filename_without_ext(file_path)
		scene_folder_path    = folder_path + filename_without_ext + utility.path_separator()

		print("-------------------------------------------------------------")
		print("exporting Photon scene to <%s>" % scene_folder_path)

		utility.create_folder(scene_folder_path)

		self.__sdlconsole = SdlConsole(scene_folder_path)
		self.__sdlconsole.start()

	def end(self):

		self.__sdlconsole.finish()

		print("exporting complete")
		print("-------------------------------------------------------------")

	def exportCamera(self, cameraType, fovDegrees, position, direction, upDirection):

		# TODO: check camera type

		position    = utility.to_photon_vec3(position)
		direction   = utility.to_photon_vec3(direction)
		upDirection = utility.to_photon_vec3(upDirection)

		clauze = clause.Vector3Clause()

		command = RawCommand()
		command.append_string(
			"""## camera(%s) [real fov-degree %.8f] %s %s %s \n""" %
			(cameraType, fovDegrees,
			 clauze.set_name("position").set_data(position).to_sdl_fragment(),
			 clauze.set_name("direction").set_data(direction).to_sdl_fragment(),
			 clauze.set_name("up-axis").set_data(upDirection).to_sdl_fragment())
		)
		self.__sdlconsole.queue_command(command)

	def exportGeometry(self, geometryType, geometryName, **keywordArgs):

		if geometryType == "triangle-mesh":
			command = RawCommand()
			command.append_string("-> geometry(triangle-mesh) %s \n" % ("\"@" + geometryName + "\""))

			positions = ""
			for position in keywordArgs["positions"]:
				triPosition = self.__blendToPhotonVector(position)
				positions += "\"%.8f %.8f %.8f\" " %(triPosition.x, triPosition.y, triPosition.z)

			texCoords = ""
			for texCoord in keywordArgs["texCoords"]:
				texCoords += "\"%.8f %.8f %.8f\" " %(texCoord[0], texCoord[1], texCoord[2])

			normals = ""
			for normal in keywordArgs["normals"]:
				triNormal = self.__blendToPhotonVector(normal)
				normals += "\"%.8f %.8f %.8f\" " %(triNormal.x, triNormal.y, triNormal.z)


			command.append_string("[vector3r-array positions {%s}]\n"           % positions)
			command.append_string("[vector3r-array texture-coordinates {%s}]\n" % texCoords)
			command.append_string("[vector3r-array normals {%s}]\n"             % normals)
			self.__sdlconsole.queue_command(command)

		elif geometryType == "rectangle":

			# TODO: width & height may correspond to different axes in Blender and Photon-v2

			command = RawCommand()
			command.append_string(
				"-> geometry(rectangle) %s [real width %.8f] [real height %.8f]\n" %
				("\"@" + geometryName + "\"", keywordArgs["width"], keywordArgs["height"])
			)
			self.__sdlconsole.queue_command(command)

		else:
			print("warning: geometry (%s) with type %s is not supported, not exporting" % (geometryName, geometryType))

	def exportMaterial(self, b_context, material_name, b_material):

		command = RawCommand()
		if not b_context.scene.ph_use_cycles_material:
			command.append_string(ui.material.to_sdl(b_material, self.__sdlconsole, material_name))
		else:
			command.append_string(export.cycles_material.to_sdl(b_material, self.__sdlconsole, material_name))
		self.__sdlconsole.queue_command(command)

	def exportLightSource(self, lightSourceType, lightSourceName, **keywordArgs):

		if lightSourceType == "area":

			emittedRadiance = keywordArgs["emittedRadiance"]
			command = RawCommand()
			command.append_string(
				"-> light-source(area) %s [vector3r emitted-radiance \"%.8f %.8f %.8f\"]\n" %
				("\"@" + lightSourceName + "\"", emittedRadiance[0], emittedRadiance[1], emittedRadiance[2])
			)
			self.__sdlconsole.queue_command(command)

		else:
			print("warning: light source (%s) with type %s is unsuppoprted, not exporting"
				  %("\"@" + lightSourceName + "\"", lightSourceType))


	def exportActorLight(self, actorLightName, lightSourceName, geometryName, materialName, position, rotation, scale):

		# TODO: check non-uniform scale

		command = RawCommand()

		position = self.__blendToPhotonVector(position)
		rotation = self.__blendToPhotonQuaternion(rotation)
		scale    = self.__blendToPhotonVector(scale)

		if lightSourceName != None:
			command.append_string("-> actor(light) %s [light-source light-source %s] "
						 %("\"@" + actorLightName + "\"", "\"@" + lightSourceName + "\""))
		else:
			print("warning: expecting a non-None light source name for actor-light %s, not exporting" %(actorLightName))
			return

		if geometryName != None:
			command.append_string("[geometry geometry %s] " %("\"@" + geometryName + "\""))

		if materialName != None:
			command.append_string("[material material %s] " %("\"@" + materialName + "\""))

		command.append_string("\n")

		command.append_string("-> actor(light) translate(%s) [vector3r factor \"%.8f %.8f %.8f\"]\n"
					 %("\"@" + actorLightName + "\"", position.x, position.y, position.z))
		command.append_string("-> actor(light) scale    (%s) [vector3r factor \"%.8f %.8f %.8f\"]\n"
					 %("\"@" + actorLightName + "\"", scale.x, scale.y, scale.z))
		command.append_string("-> actor(light) rotate   (%s) [quaternionR factor \"%.8f %.8f %.8f %.8f\"]\n"
					 %("\"@" + actorLightName + "\"", rotation.x, rotation.y, rotation.z, rotation.w))

		self.__sdlconsole.queue_command(command)


	def exportActorModel(self, actorModelName, geometryName, materialName, position, rotation, scale):

		if (actorModelName == None) or (geometryName == None) or (materialName == None):
			print("warning: no name should be none, not exporting")
			return

		command = RawCommand()

		position = self.__blendToPhotonVector(position)
		rotation = self.__blendToPhotonQuaternion(rotation)
		scale    = self.__blendToPhotonVector(scale)

		command.append_string("-> actor(model) %s [geometry geometry %s] [material material %s]\n"
					 %("\"@" + actorModelName + "\"", "\"@" + geometryName + "\"", "\"@" + materialName + "\""))

		command.append_string("-> actor(model) translate(%s) [vector3r factor \"%.8f %.8f %.8f\"]\n"
					 %("\"@" + actorModelName + "\"", position.x, position.y, position.z))
		command.append_string("-> actor(model) scale    (%s) [vector3r factor \"%.8f %.8f %.8f\"]\n"
					 %("\"@" + actorModelName + "\"", scale.x, scale.y, scale.z))
		command.append_string("-> actor(model) rotate   (%s) [quaternionR factor \"%.8f %.8f %.8f %.8f\"]\n"
					 %("\"@" + actorModelName + "\"", rotation.x, rotation.y, rotation.z, rotation.w))

		self.__sdlconsole.queue_command(command)

	def exportRaw(self, rawText):
		command = RawCommand()
		command.append_string(rawText)
		self.__sdlconsole.queue_command(command)

	def __blendToPhotonVector(self, blenderVector):
		photonVector = mathutils.Vector((blenderVector.y,
										 blenderVector.z,
										 blenderVector.x))
		return photonVector

	def __blendToPhotonQuaternion(self, blenderQuaternion):
		# initializer is like mathutils.Quaternion(w, x, y, z)
		photonQuaternion = mathutils.Quaternion((blenderQuaternion.w,
												 blenderQuaternion.y,
												 blenderQuaternion.z,
												 blenderQuaternion.x))
		return photonQuaternion


def export_geometry(exporter, geometryName, mesh, faces):

	# all UV maps for tessellated faces
	uvMaps = mesh.tessface_uv_textures

	uvLayers = None

	if len(uvMaps) > 0:
		if uvMaps.active != None:
			uvLayers = uvMaps.active.data
		else:
			print("warning: mesh (%s) has %d uv maps, but no one is active (no uv map will be exported)" %(geometryName, len(uvMaps)))

		if len(uvMaps) > 1:
			print("warning: mesh (%s) has %d uv maps, only the active one is exported" %(geometryName, len(uvMaps)))

	triPositions = []
	triTexCoords = []
	triNormals   = []

	for face in faces:

		faceVertexIndices = [0, 1, 2]

		# identify and triangulate quads (assuming coplanar & CCW)
		if len(face.vertices) > 3:
			if len(face.vertices) == 4:
				faceVertexIndices.extend([0, 2, 3])
			else:
				print("warning: face of mesh %s consists more than 4 vertices which is unsupported, ignoring" %(geometryName))
				continue

		# gather triangle data
		for faceVertexIndex in faceVertexIndices:
			vertexIndex = face.vertices[faceVertexIndex]
			triVertex = mesh.vertices[vertexIndex]

			triPosition = triVertex.co
			triNormal   = triVertex.normal if face.use_smooth else face.normal
			triTexCoord = [0, 0, 0]

			if uvLayers != None:
				faceUvLayer = uvLayers[face.index]
				triTexCoord[0] = faceUvLayer.uv[faceVertexIndex][0]
				triTexCoord[1] = faceUvLayer.uv[faceVertexIndex][1]

			triPositions.append(triPosition)
			triTexCoords.append(triTexCoord)
			triNormals.append(triNormal)

	exporter.exportGeometry("triangle-mesh", geometryName,
							positions = triPositions,
							texCoords = triTexCoords,
							normals   = triNormals)


def export_material(exporter, b_context, material_name, b_material):

	exporter.exportMaterial(b_context, material_name, b_material)


def export_object_mesh(exporter, b_context, obj):

	scene = b_context.scene

	if len(obj.data.materials) != 0:

		# this creates a temporary mesh data with all modifiers applied for exporting
		# (don't forget to delete it after exporting)
		mesh = obj.to_mesh(scene, apply_modifiers = True, settings = "RENDER", calc_tessface = True)

		if mesh == None:
			print("warning: mesh object %s cannot convert to mesh, not exporting" %(obj.name))
			bpy.data.meshes.remove(mesh)
			return

		materialIdFacesMap = {}

		# group faces with the same material, then export each face-material pair as a Photon-v2's actor

		for face in mesh.tessfaces:
			# note that this index refers to material slots (their stack order on the UI)
			matId = face.material_index

			if matId not in materialIdFacesMap.keys():
				materialIdFacesMap[matId] = []

			materialIdFacesMap[matId].append(face)

		for matId in materialIdFacesMap.keys():

			material = obj.material_slots[matId].material
			faces    = materialIdFacesMap[matId]

			# a material slot can be empty, this check is necessary
			if material == None:
				print("warning: no material is in mesh object %s's material slot %d, not exporting" %(obj.name, matId))
				continue

			# same material can be in different slots, with slot index as suffix we can ensure unique material
			# names (required by Photon-v2 for creating unique materials)
			geometryName = mangled_geometry_name(obj, mesh.name, str(matId))
			materialName = mangled_material_name(obj, mesh.name + "_" + material.name, str(matId))

			export_geometry(exporter, geometryName, mesh, faces)
			export_material(exporter, b_context, materialName, material)

			actorType = None
			actorName = None

			# creating actor (can be either model or light depending on emissivity)
			pos, rot, scale = obj.matrix_world.decompose()

			if material.ph_is_emissive:

				lightSourceName = mangled_light_source_name(obj, mesh.name, str(matId))
				actorLightName  = mangled_actor_light_name(obj, "", str(matId))

				exporter.exportLightSource("area", lightSourceName, emittedRadiance = material.ph_emitted_radiance)
				exporter.exportActorLight(actorLightName, lightSourceName, geometryName, materialName, pos, rot, scale)

			else:

				actorModelName = mangled_actor_model_name(obj, "", str(matId))

				exporter.exportActorModel(actorModelName, geometryName, materialName, pos, rot, scale)

		# delete the temporary mesh for exporting
		bpy.data.meshes.remove(mesh)

	else:
		print("warning: mesh object (%s) has no material, not exporting" %(obj.name))


def export_object_lamp(exporter, b_context, obj):

	lamp = obj.data

	if lamp.type == "AREA":

		lightMaterialName = mangled_material_name(obj, lamp.name, "")
		lightGeometryName = mangled_geometry_name(obj, lamp.name, "")
		lightSourceName   = mangled_light_source_name(obj, lamp.name, "")
		actorLightName    = mangled_actor_light_name(obj, "blenderLamp", "")

		# In Blender's Lamp, under Area category, only Square and Rectangle shape are available.
		# (which are both a rectangle in Photon-v2)
		recWidth  = lamp.size
		recHeight = lamp.size_y if lamp.shape == "RECTANGLE" else lamp.size
		exporter.exportGeometry("rectangle", lightGeometryName, width = recWidth, height = recHeight)

		# HACK: assume the Lamp uses this material
		b_material = bpy.data.materials.new(lightMaterialName)
		exporter.exportMaterial(b_context, lightMaterialName, b_material)
		bpy.data.materials.remove(b_material)

		# use lamp's color attribute as emitted radiance
		exporter.exportLightSource("area", lightSourceName, emittedRadiance = lamp.color)

		# creating actor-light, also convert transformation to Photon-v2's coordinate system

		pos, rot, scale = obj.matrix_world.decompose()

		# Blender's rectangle area light is in its xy-plane (facing -z axis) by default, 
		# while Photon's rectangle is in Blender's yz-plane (facing +x axis); these 
		# rotations accounts for such difference
		rot = rot * mathutils.Quaternion((1.0, 0.0, 0.0), math.radians(90.0))
		rot = rot * mathutils.Quaternion((0.0, 0.0, 1.0), math.radians(-90.0))

		exporter.exportActorLight(actorLightName, lightSourceName, lightGeometryName, lightMaterialName, pos, rot, scale)

	else:
		print("warning: lamp (%s) type (%s) is unsupported, not exporting" %(lamp.name, lamp.type))


def export_camera(exporter, obj, scene):

	camera = obj.data

	if camera.type == "PERSP":

		pos, rot, scale = obj.matrix_world.decompose()
		if abs(scale.x - 1.0) > 0.0001 or abs(scale.y - 1.0) > 0.0001 or abs(scale.z - 1.0) > 0.0001:
			print("warning: camera (%s) contains scale factor, ignoring" % camera.name)

		# Blender's camera intially pointing (0, 0, -1) with up (0, 1, 0) in its math.py system
		# (also note that Blender's quaternion works this way, does not require q*v*q').
		cam_dir     = rot * mathutils.Vector((0, 0, -1))
		cam_up_dir  = rot * mathutils.Vector((0, 1, 0))
		fov_degrees = 70.0

		lens_unit = camera.lens_unit
		if lens_unit == "FOV":
			fov_degrees = math.degrees(camera.angle)
		elif lens_unit == "MILLIMETERS":
			sensor_width = camera.sensor_width
			focal_length = camera.lens
			fov_degrees  = math.degrees(math.atan((sensor_width / 2.0) / focal_length)) * 2.0
		else:
			print("warning: camera (%s) with lens unit %s is unsupported, not exporting"
			      % (camera.name, camera.lens_unit))

		exporter.exportCamera("pinhole", fov_degrees, pos, cam_dir, cam_up_dir)

	else:
		print("warning: camera (%s) type (%s) is unsupported, not exporting" % (camera.name, camera.type))


def export_core_commands(exporter, context):
	objs = context.scene.objects
	for obj in objs:
		if obj.type == "CAMERA":
			export_camera(exporter, obj, context.scene)

	meta_info = meta.MetaGetter(context)

	exporter.exportRaw("## film(hdr-rgb) [integer width %s] [integer height %s] [string filter-name %s]\n"
	                   % (meta_info.render_width_px(),
	                      meta_info.render_height_px(),
	                      meta_info.sample_filter_name()))

	exporter.exportRaw("## sample-generator(stratified) [integer sample-amount %s] "
	                   "[integer num-strata-2d-x %s] [integer num-strata-2d-y %s]\n"
	                   % (meta_info.spp(), meta_info.render_width_px(), meta_info.render_height_px()))

	exporter.exportRaw("## integrator(backward-path) \n")


def export_world_commands(exporter, b_context):
	scene = b_context.scene
	objs = scene.objects
	for obj in objs:
		if obj.type == "MESH":
			print("exporting mesh " + obj.name)
			export_object_mesh(exporter, b_context, obj)
		elif obj.type == "LAMP":
			export_object_lamp(exporter, b_context, obj)
		elif obj.type == "CAMERA":
			# do nothing since it belongs to core command
			continue
		else:
			print("warning: object (%s) type (%s) is not supported, not exporting" %(obj.name, obj.type))


class P2Exporter(Operator, ExportHelper):
	"""export the scene to some Photon-v2 readable format"""
	bl_idname = "object.p2_exporter"
	bl_label  = "export p2"

	# ExportHelper mixin class uses this
	filename_ext = ""

	# filter_glob = StringProperty(
	# 	default="*.p2",
	# 	options={"HIDDEN"},
	# )

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

	def execute(self, b_context):

		exporter = Exporter(self.filepath)
		exporter.begin()

		export_core_commands(exporter, b_context)
		export_world_commands(exporter, b_context)

		exporter.end()

		return {"FINISHED"}


# Only needed if you want to add into a dynamic menu
def menu_func_export(self, context):
	self.layout.operator(P2Exporter.bl_idname, text = "Photon Scene (.p2)")


def register():
	bpy.utils.register_class(P2Exporter)
	bpy.types.INFO_MT_file_export.append(menu_func_export)


def unregister():
	bpy.types.INFO_MT_file_export.remove(menu_func_export)
	bpy.utils.unregister_class(P2Exporter)


if __name__ == "__main__":
	register()

	# test call
	bpy.ops.object.p2_exporter("INVOKE_DEFAULT")
