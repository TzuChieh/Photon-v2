from ..psdl import clause
from ..psdl.sdlconsole import SdlConsole
from ..psdl.cmd import RawCommand
from ..psdl.lightcmd import ModelLightCreator
from .. import utility
from ..utility import meta
from . import ui
from . import export
from .export import naming
from . import light
from . import node
from ..psdl import actorcmd
from ..psdl import sdlresource

from ..psdl.pysdl import (
	SDLReal,
	SDLInteger,
	SDLVector3,
	SDLQuaternion,
	SDLString,
	SDLVector3Array,
	SDLReference,
	SDLImage,
	SDLGeometry,
	SDLMaterial,
	SDLLightSource,
	SDLSampleGenerator,
	SDLCamera,
	SDLRenderer)

from ..psdl.pysdl import (
	TriangleMeshGeometryCreator,
	RectangleGeometryCreator,
	ModelActorCreator,
	LightActorCreator,
	ModelActorTranslate,
	ModelActorRotate,
	ModelActorScale,
	LightActorTranslate,
	LightActorRotate,
	LightActorScale,
	PinholeCameraCreator,
	ThinLensCameraCreator,
	EqualSamplingRendererCreator,
	PmRendererCreator,
	ModelLightSourceCreator,
	DomeActorCreator,
	DomeActorRotate,
	StratifiedSampleGeneratorCreator,
	AttributeRendererCreator)

import bpy
import mathutils

# ExportHelper is a helper class, defines filename and
# invoke() function which calls the file selector.
from bpy_extras.io_utils import ExportHelper
from bpy.props import StringProperty, BoolProperty, EnumProperty
from bpy.types import Operator

import math
import shutil

#class MaterialExportStatus:
#
#	def __init__(self, emission_image_command = None):
#		self.emission_image_command = emission_image_command


class Exporter:

	def __init__(self, file_path):
		self.__file_path  = file_path
		self.__sdlconsole = None

	# TODO: should not expose console
	def get_sdlconsole(self):
		return self.__sdlconsole

	def begin(self, scene_name):

		file_path            = self.__file_path
		folder_path          = utility.get_folder_path(file_path)
		filename_without_ext = utility.get_filename_without_ext(file_path)
		scene_folder_path    = folder_path + filename_without_ext + utility.path_separator()

		print("-------------------------------------------------------------")
		print("exporting Photon scene to <%s>" % scene_folder_path)

		utility.create_folder(scene_folder_path)

		self.__sdlconsole = SdlConsole(scene_folder_path, scene_name)
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

		creator = PinholeCameraCreator()
		creator.set_fov_degree(SDLReal(fovDegrees))
		creator.set_position(SDLVector3(position))
		creator.set_direction(SDLVector3(direction))
		creator.set_up_axis(SDLVector3(upDirection))
		self.__sdlconsole.queue_command(creator)

	def export_triangle_mesh(self, geometryType, geometryName, **keywordArgs):

		if geometryType == "triangle-mesh":
			creator = TriangleMeshGeometryCreator()
			creator.set_data_name(geometryName)

			positions = SDLVector3Array()
			for position in keywordArgs["positions"]:
				triPosition = self.__blendToPhotonVector(position)
				positions.add(triPosition)
			creator.set_positions(positions)

			tex_coords = SDLVector3Array()
			for texCoord in keywordArgs["texCoords"]:
				tex_coords.add(texCoord)
			creator.set_texture_coordinates(tex_coords)

			normals = SDLVector3Array()
			for normal in keywordArgs["normals"]:
				triNormal = self.__blendToPhotonVector(normal)
				normals.add(triNormal)
			creator.set_normals(normals)

			self.__sdlconsole.queue_command(creator)

		elif geometryType == "rectangle":

			# TODO: width & height may correspond to different axes in Blender and Photon-v2

			creator = RectangleGeometryCreator()
			creator.set_data_name(geometryName)
			creator.set_width(SDLReal(keywordArgs["width"]))
			creator.set_height(SDLReal(keywordArgs["height"]))
			self.__sdlconsole.queue_command(creator)

		else:
			print("warning: geometry (%s) with type %s is not supported, not exporting" % (geometryName, geometryType))

	def exportMaterial(self, b_context, material_name, b_material):

		if not b_context.scene.ph_use_cycles_material:
			use_node_tree = b_material.ph_node_tree_name != ""
			if use_node_tree:
				return node.to_sdl(material_name, b_material, self.get_sdlconsole())
			else:
				# BROKEN CODE
				# command = RawCommand()
				# command.append_string(ui.material.to_sdl(b_material, self.__sdlconsole, material_name))
				# self.__sdlconsole.queue_command(command)
				# return node.MaterialNodeTranslateResult()
				return None
		else:
			# BROKEN CODE
			# translate_result = export.cycles_material.translate(b_material, self.__sdlconsole, material_name)
			# if not translate_result.is_valid():
			# 	print("warning: cycles material %s translation failed" % material_name)
			# return node.MaterialNodeTranslateResult()
			return None


	# def exportLightSource(self, lightSourceType, lightSourceName, **keywordArgs):
	#
	# 	if lightSourceType == "model":
	#
	# 		emittedRadiance = keywordArgs["emittedRadiance"]
	# 		command = RawCommand()
	# 		command.append_string(
	# 			"-> light-source(model) %s [vector3r emitted-radiance \"%.8f %.8f %.8f\"]\n" %
	# 			("\"@" + lightSourceName + "\"", emittedRadiance[0], emittedRadiance[1], emittedRadiance[2])
	# 		)
	# 		self.__sdlconsole.queue_command(command)
	#
	# 	else:
	# 		print("warning: light source (%s) with type %s is unsuppoprted, not exporting"
	# 			  %("\"@" + lightSourceName + "\"", lightSourceType))


	def exportActorLight(self, actorLightName, lightSourceName, geometryName, materialName, position, rotation, scale):

		# TODO: check non-uniform scale

		position = self.__blendToPhotonVector(position)
		rotation = self.__blendToPhotonQuaternion(rotation)
		scale    = self.__blendToPhotonVector(scale)

		if lightSourceName != None:
			creator = LightActorCreator()
			creator.set_data_name(actorLightName)
			creator.set_light_source(SDLLightSource(lightSourceName))
			self.__sdlconsole.queue_command(creator)
		else:
			print("warning: expecting a non-None light source name for actor-light %s, not exporting" %(actorLightName))
			return

		# if geometryName != None:
		# 	command.append_string("[geometry geometry %s] " %("\"@" + geometryName + "\""))
		#
		# if materialName != None:
		# 	command.append_string("[material material %s] " %("\"@" + materialName + "\""))

		translator = LightActorTranslate()
		translator.set_target_name(actorLightName)
		translator.set_factor(SDLVector3(position))
		self.__sdlconsole.queue_command(translator)

		rotator = LightActorRotate()
		rotator.set_target_name(actorLightName)
		rotator.set_factor(SDLQuaternion((rotation.x, rotation.y, rotation.z, rotation.w)))
		self.__sdlconsole.queue_command(rotator)

		scaler = LightActorScale()
		scaler.set_target_name(actorLightName)
		scaler.set_factor(SDLVector3(scale))
		self.__sdlconsole.queue_command(scaler)


	def exportActorModel(self, actorModelName, geometryName, materialName, position, rotation, scale):

		if (actorModelName == None) or (geometryName == None) or (materialName == None):
			print("warning: no name should be none, not exporting")
			return

		position = self.__blendToPhotonVector(position)
		rotation = self.__blendToPhotonQuaternion(rotation)
		scale    = self.__blendToPhotonVector(scale)

		creator = ModelActorCreator()
		creator.set_data_name(actorModelName)
		creator.set_geometry(SDLGeometry(geometryName))
		creator.set_material(SDLMaterial(materialName))
		self.__sdlconsole.queue_command(creator)

		translator = ModelActorTranslate()
		translator.set_target_name(actorModelName)
		translator.set_factor(SDLVector3(position))
		self.__sdlconsole.queue_command(translator)

		rotator = ModelActorRotate()
		rotator.set_target_name(actorModelName)
		rotator.set_factor(SDLQuaternion((rotation.x, rotation.y, rotation.z, rotation.w)))
		self.__sdlconsole.queue_command(rotator)

		scaler = ModelActorScale()
		scaler.set_target_name(actorModelName)
		scaler.set_factor(SDLVector3(scale))
		self.__sdlconsole.queue_command(scaler)

	# def exportRaw(self, rawText):
	# 	command = RawCommand()
	# 	command.append_string(rawText)
	# 	self.__sdlconsole.queue_command(command)

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

	def export_geometry(self, geometryName, mesh, faces):

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

		self.export_triangle_mesh("triangle-mesh", geometryName,
		                          positions = triPositions,
		                          texCoords = triTexCoords,
		                          normals   = triNormals)

	def export_object_mesh(self, b_context, obj):

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
				geometryName = naming.mangled_geometry_name(obj, mesh.name, str(matId))
				materialName = naming.mangled_material_name(obj, mesh.name + "_" + material.name, str(matId))

				self.export_geometry(geometryName, mesh, faces)
				mat_translate_result = self.exportMaterial(b_context, materialName, material)

				# creating actor (can be either model or light depending on emissivity)
				pos, rot, scale = obj.matrix_world.decompose()

				if mat_translate_result.is_surface_emissive():

					lightSourceName = naming.mangled_light_source_name(obj, mesh.name, str(matId))
					actorLightName  = naming.mangled_actor_light_name(obj, "", str(matId))

					creator = ModelLightSourceCreator()
					creator.set_data_name(lightSourceName)
					creator.set_emitted_radiance(SDLImage(mat_translate_result.surface_emi_res_name))
					creator.set_geometry(SDLGeometry(geometryName))
					creator.set_material(SDLMaterial(materialName))
					self.get_sdlconsole().queue_command(creator)

					self.exportActorLight(actorLightName, lightSourceName, geometryName, materialName, pos, rot, scale)

				# elif material.ph_is_emissive:
				#
				# 	# FIXME: broken code here
				#
				# 	lightSourceName = naming.mangled_light_source_name(obj, mesh.name, str(matId))
				# 	actorLightName  = naming.mangled_actor_light_name(obj, "", str(matId))
				#
				# 	self.exportLightSource("model", lightSourceName, emittedRadiance = material.ph_emitted_radiance)
				# 	self.exportActorLight(actorLightName, lightSourceName, geometryName, materialName, pos, rot, scale)

				else:

					actorModelName = naming.mangled_actor_model_name(obj, "", str(matId))

					self.exportActorModel(actorModelName, geometryName, materialName, pos, rot, scale)

			# delete the temporary mesh for exporting
			bpy.data.meshes.remove(mesh)

		else:
			print("warning: mesh object (%s) has no material, not exporting" %(obj.name))

	def export_camera(self, obj, scene):

		b_camera = obj.data

		if b_camera.type == "PERSP":

			pos, rot, scale = obj.matrix_world.decompose()
			if abs(scale.x - 1.0) > 0.0001 or abs(scale.y - 1.0) > 0.0001 or abs(scale.z - 1.0) > 0.0001:
				print("warning: camera (%s) contains scale factor, ignoring" % b_camera.name)

			# Blender's camera intially pointing (0, 0, -1) with up (0, 1, 0) in its math.py system
			# (also note that Blender's quaternion works this way, does not require q*v*q').
			cam_dir     = rot @ mathutils.Vector((0, 0, -1))
			cam_up_dir  = rot @ mathutils.Vector((0, 1, 0))
			fov_degrees = 70.0

			lens_unit = b_camera.lens_unit
			if lens_unit == "FOV":
				fov_degrees = math.degrees(b_camera.angle)
			elif lens_unit == "MILLIMETERS":
				sensor_width = b_camera.sensor_width
				focal_length = b_camera.lens
				fov_degrees  = math.degrees(math.atan((sensor_width / 2.0) / focal_length)) * 2.0
			else:
				print("warning: camera (%s) with lens unit %s is unsupported, not exporting"
				      % (b_camera.name, b_camera.lens_unit))

			# HACK
			if not b_camera.ph_has_dof:
				self.exportCamera("pinhole", fov_degrees, pos, cam_dir, cam_up_dir)
			else:
				position = utility.to_photon_vec3(pos)
				direction = utility.to_photon_vec3(cam_dir)
				up_direction = utility.to_photon_vec3(cam_up_dir)

				creator = ThinLensCameraCreator()
				creator.set_fov_degree(SDLReal(fov_degrees))
				creator.set_position(SDLVector3(position))
				creator.set_direction(SDLVector3(direction))
				creator.set_up_axis(SDLVector3(up_direction))
				creator.set_lens_radius_mm(SDLReal(b_camera.ph_lens_radius_mm))
				creator.set_focal_distance_mm(SDLReal(b_camera.ph_focal_meters * 1000))
				self.__sdlconsole.queue_command(creator)

		else:
			print("warning: camera (%s) type (%s) is unsupported, not exporting" % (camera.name, camera.type))

	def export_world(self, b_world):

		if b_world.ph_envmap_file_path == "":
			return

		actor_name = "ph_" + b_world.name

		creator = DomeActorCreator()
		creator.set_data_name(actor_name)

		envmap_path  = bpy.path.abspath(b_world.ph_envmap_file_path)
		envmap_sdlri = sdlresource.SdlResourceIdentifier()
		envmap_sdlri.append_folder(b_world.name + "_data")
		envmap_sdlri.set_file(utility.get_filename(envmap_path))
		creator.set_env_map(SDLString(envmap_sdlri.get_identifier()))

		# copy the envmap to scene folder
		self.get_sdlconsole().create_resource_folder(envmap_sdlri)
		dst_path = utility.get_appended_path(self.get_sdlconsole().get_working_directory(),
		                                     envmap_sdlri.get_path())
		shutil.copyfile(envmap_path, dst_path)

		self.get_sdlconsole().queue_command(creator)

		rotation = DomeActorRotate()
		rotation.set_target_name(actor_name)
		rotation.set_axis(SDLVector3((0, 1, 0)))
		rotation.set_degree(SDLReal(b_world.ph_envmap_degrees))
		self.get_sdlconsole().queue_command(rotation)

	def export_core_commands(self, context):

		b_scene  = context.scene
		b_camera = b_scene.camera

		# a scene may contain multiple cameras, export the active one only
		if b_camera is not None:
			self.export_camera(b_camera, b_scene)
		else:
			print("warning: no active camera")

		meta_info = meta.MetaGetter(context)

		sample_generator = StratifiedSampleGeneratorCreator()
		sample_generator.set_sample_amount(SDLInteger(meta_info.spp()))
		self.get_sdlconsole().queue_command(sample_generator)

		render_method = meta_info.render_method()

		renderer = None

		if render_method == "BVPT" or render_method == "BNEEPT" or render_method == "BVPTDL":

			renderer = EqualSamplingRendererCreator()
			renderer.set_filter_name(SDLString(meta_info.sample_filter_name()))
			renderer.set_estimator(SDLString(meta_info.integrator_type_name()))

		elif render_method == "VPM":

			renderer = PmRendererCreator()
			renderer.set_mode(SDLString("vanilla"))
			renderer.set_num_photons(SDLInteger(b_scene.ph_render_num_photons))
			renderer.set_num_samples_per_pixel(SDLInteger(b_scene.ph_render_num_spp_pm))
			renderer.set_radius(SDLReal(b_scene.ph_render_kernel_radius))

		elif render_method == "PPM" or render_method == "SPPM":

			mode_name = "progressive" if render_method == "PPM" else "stochastic-progressive"
			renderer = PmRendererCreator()
			renderer.set_mode(SDLString(mode_name))
			renderer.set_num_photons(SDLInteger(b_scene.ph_render_num_photons))
			renderer.set_num_samples_per_pixel(SDLInteger(b_scene.ph_render_num_spp_pm))
			renderer.set_radius(SDLReal(b_scene.ph_render_kernel_radius))
			renderer.set_num_passes(SDLInteger(b_scene.ph_render_num_passes))

		elif render_method == "ATTRIBUTE":

			renderer = AttributeRendererCreator()

		elif render_method == "CUSTOM":

			custom_renderer_sdl_command = RawCommand()
			custom_renderer_sdl_command.append_string(b_scene.ph_render_custom_sdl)
			custom_renderer_sdl_command.append_string("\n")
			self.get_sdlconsole().queue_command(custom_renderer_sdl_command)

		else:
			print("warning: render method %s is not supported" % render_method)

		if renderer is not None:

			renderer.set_width(SDLInteger(meta_info.render_width_px()))
			renderer.set_height(SDLInteger(meta_info.render_height_px()))

			if b_scene.ph_use_crop_window:
				renderer.set_rect_x(SDLInteger(b_scene.ph_crop_min_x))
				renderer.set_rect_y(SDLInteger(b_scene.ph_crop_min_y))
				renderer.set_rect_w(SDLInteger(b_scene.ph_crop_width))
				renderer.set_rect_h(SDLInteger(b_scene.ph_crop_height))

			self.get_sdlconsole().queue_command(renderer)

	# TODO: write/flush commands to disk once a while (reducing memory usage)
	def export_world_commands(self, b_context):
		scene = b_context.scene
		objs = scene.objects
		for obj in objs:
			if obj.type == "MESH":
				print("exporting mesh " + obj.name)
				self.export_object_mesh(b_context, obj)
			elif obj.type == "LIGHT":
				light.to_sdl_commands(obj, self.get_sdlconsole())
			elif obj.type == "CAMERA":
				# do nothing since it belongs to core command
				continue
			elif obj.type == "ARMATURE":
				# not visible
				continue
			else:
				print("warning: object (%s) type (%s) is not supported, not exporting" %(obj.name, obj.type))

		b_world = scene.world
		if b_world is not None:
			self.export_world(b_world)


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

	is_export_animation_requested = BoolProperty(
		name        = "Export Animation",
		description = "Export each frame as a separate scene file.",
		default     = False,
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

	def execute(self, b_context):

		scene = b_context.scene

		if not self.is_export_animation_requested:

			exporter = Exporter(self.filepath)
			exporter.begin("scene")

			exporter.export_core_commands(b_context)
			exporter.export_world_commands(b_context)

			exporter.end()

			return {"FINISHED"}

		else:

			for frame_number in range(scene.frame_start, scene.frame_end + 1):

				scene.frame_set(frame_number)

				exporter = Exporter(self.filepath)
				exporter.begin("scene_" + str(frame_number).zfill(6))

				exporter.export_core_commands(b_context)
				exporter.export_world_commands(b_context)

				exporter.end()

			return {"FINISHED"}


# Only needed if you want to add into a dynamic menu
def menu_func_export(self, context):
	self.layout.operator(P2Exporter.bl_idname, text = "Photon Scene (.p2)")


def register():
	bpy.utils.register_class(P2Exporter)
	bpy.types.TOPBAR_MT_file_export.append(menu_func_export)


def unregister():
	bpy.types.TOPBAR_MT_file_export.remove(menu_func_export)
	bpy.utils.unregister_class(P2Exporter)


if __name__ == "__main__":
	register()

	# test call
	bpy.ops.object.p2_exporter("INVOKE_DEFAULT")
