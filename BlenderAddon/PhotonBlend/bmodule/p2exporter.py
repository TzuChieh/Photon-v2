from ..psdl.sdlconsole import SdlConsole
from ..psdl.cmd import RawCommand
from .. import utility
from ..utility import meta, blender
from .export import naming
from . import lights
from .material import node
from ..psdl import sdlresource
from .mesh import triangle_mesh
from . import scene
from . import material

from ..psdl.pysdl import (
	SDLReal,
	SDLInteger,
	SDLVector3,
	SDLQuaternion,
	SDLString,
	SDLImage,
	SDLGeometry,
	SDLMaterial,
	SDLLightSource)

from ..psdl.pysdl import (
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
from bpy.props import BoolProperty
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

	def exportMaterial(self, b_context, material_name, b_material):

		print("name = %s" % b_material.name)
		print("evaluated = %s" % b_material.ph_node_tree_name)
		print("original = %s" % b_material.original.ph_node_tree_name)

		if not b_context.scene.ph_use_cycles_material:
			use_node_tree = b_material.ph_node_tree_name != ""
			if use_node_tree:
				return node.to_sdl(material_name, b_material, self.get_sdlconsole())
			else:
				print("not using node tree")
				# BROKEN CODE
				# command = RawCommand()
				# command.append_string(ui.matl.to_sdl(b_material, self.__sdlconsole, material_name))
				# self.__sdlconsole.queue_command(command)
				# return node.MaterialNodeTranslateResult()
				return None
		else:
			print("using cycles matl")
			# BROKEN CODE
			# translate_result = export.cycles_material.translate(b_material, self.__sdlconsole, material_name)
			# if not translate_result.is_valid():
			# 	print("warning: cycles matl %s translation failed" % material_name)
			# return node.MaterialNodeTranslateResult()
			return None

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
		# 	command.append_string("[matl matl %s] " %("\"@" + materialName + "\""))

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

	def export_object_mesh(self, b_context, b_depsgraph, b_obj):

		if len(b_obj.data.materials) == 0:
			print("warning: mesh object (%s) has no matl, not exporting" % b_obj.name)
			return

		# FIXME: this call is only necessary in edit mode, maybe properly check this?
		# see (https://blenderartists.org/t/get-mesh-data-with-modifiers-applied-in-2-8/1163217/2)
		#
		# This creates a temporary mesh data with all modifiers applied (do not forget to delete it later).
		# b_mesh = b_obj.to_mesh()
		b_mesh = b_obj.to_mesh(preserve_all_data_layers=True, depsgraph=b_depsgraph)
		# b_mesh = b_obj.data
		if b_mesh is None:
			print("warning: cannot convert　mesh object %s　to mesh, not exporting" % b_obj.name)
			return

		# Group faces with the same matl, then export each face-matl pair as a Photon-v2's actor.

		b_mesh.calc_loop_triangles()
		b_mesh.calc_normals()

		# TODO: might be faster if using len(obj.material_slots()) for array size and simply store each loop tris array
		material_idx_loop_triangles_map = {}
		for b_loop_triangle in b_mesh.loop_triangles:

			# This index refers to matl slots (their stack order in the UI).
			material_idx = b_loop_triangle.material_index

			if material_idx not in material_idx_loop_triangles_map.keys():
				material_idx_loop_triangles_map[material_idx] = []

			material_idx_loop_triangles_map[material_idx].append(b_loop_triangle)

		for material_idx in material_idx_loop_triangles_map.keys():

			b_material = b_obj.material_slots[material_idx].material.evaluated_get(b_depsgraph)
			loop_triangles = material_idx_loop_triangles_map[material_idx]

			# A matl slot can be empty, this check is necessary.
			if b_material is None:
				print("warning: no matl is in mesh object %s's matl slot %d, not exporting" % (
					b_obj.name, material_idx))
				continue

			# Same matl can be in different slots, with slot index as suffix we can ensure unique matl
			# names (required by Photon-v2 for creating unique materials).
			geometry_name = naming.mangled_geometry_name(b_obj, b_mesh.name, str(material_idx))
			material_name = naming.mangled_material_name(b_obj, b_mesh.name + "_" + b_material.name, str(material_idx))

			# Use the active one as the UV map for export.
			# TODO: support exporting multiple or zero UV maps/layers
			b_uv_layers = b_mesh.uv_layers
			b_active_uv_layer = b_uv_layers.active

			# TODO: support mesh without uv map
			if len(b_mesh.uv_layers) == 0:
				print("warning: mesh (%s) has no uv maps, ignoring" % geometry_name)
				continue

			# TODO: support mesh without uv map
			if b_active_uv_layer is None:
				print("warning: mesh (%s) has %d uv maps, but no one is active (no uv map will be exported)" % (
					geometry_name, len(b_uv_layers)))
				continue

			# TODO: support mesh with multiple uv maps
			if len(b_mesh.uv_layers) > 1:
				print("warning: mesh (%s) has %d uv maps, only the active one is exported" % (
					geometry_name, len(b_uv_layers)))

			triangle_mesh.loop_triangles_to_sdl_triangle_mesh(
				geometry_name,
				self.__sdlconsole,
				loop_triangles,
				b_mesh.vertices,
				b_active_uv_layer.data)

			mat_translate_result = self.exportMaterial(b_context, material_name, b_material)

			# creating actor (can be either model or light depending on emissivity)
			pos, rot, scale = b_obj.matrix_world.decompose()

			if mat_translate_result.is_surface_emissive():
				light_source_name = naming.mangled_light_source_name(b_obj, b_mesh.name, str(material_idx))
				creator = ModelLightSourceCreator()
				creator.set_data_name(light_source_name)
				creator.set_emitted_radiance(SDLImage(mat_translate_result.surface_emi_res_name))
				creator.set_geometry(SDLGeometry(geometry_name))
				creator.set_material(SDLMaterial(material_name))
				self.get_sdlconsole().queue_command(creator)

				actor_light_name = naming.mangled_actor_light_name(b_obj, "", str(material_idx))
				self.exportActorLight(actor_light_name, light_source_name, geometry_name, material_name, pos, rot, scale)
			else:
				actor_model_name = naming.mangled_actor_model_name(b_obj, "", str(material_idx))
				self.exportActorModel(actor_model_name, geometry_name, material_name, pos, rot, scale)

		# delete the temporary mesh for exporting
		# FIXME: this call may be missed if any exception has thrown earlier
		b_obj.to_mesh_clear()

	def export_mesh_object(self, b_depsgraph: bpy.types.Depsgraph, b_mesh_object: bpy.types.Object):
		b_mesh = b_mesh_object.data

		if b_mesh is None:
			print("warning: mesh object (%s)　has no mesh data, not exporting" % b_mesh_object.name)
			return

		if len(b_mesh.materials) == 0:
			print("warning: mesh object (%s) has no matl, not exporting" % b_mesh_object.name)
			return

		# Group faces with the same matl, then export each face-matl pair as a Photon-v2's actor.

		b_mesh.calc_loop_triangles()
		b_mesh.calc_normals()

		# TODO: might be faster if using len(obj.material_slots()) for array size and simply store each loop tris array
		material_idx_loop_triangles_map = {}
		for b_loop_triangle in b_mesh.loop_triangles:

			# This index refers to matl slots (their stack order in the UI).
			material_idx = b_loop_triangle.material_index

			if material_idx not in material_idx_loop_triangles_map.keys():
				material_idx_loop_triangles_map[material_idx] = []

			material_idx_loop_triangles_map[material_idx].append(b_loop_triangle)

		for material_idx in material_idx_loop_triangles_map.keys():

			b_material = b_obj.material_slots[material_idx].material.evaluated_get(b_depsgraph)
			loop_triangles = material_idx_loop_triangles_map[material_idx]

			# A matl slot can be empty, this check is necessary.
			if b_material is None:
				print("warning: no matl is in mesh object %s's matl slot %d, not exporting" % (
					b_mesh_object.name, material_idx))
				continue

			# Same matl can be in different slots, with slot index as suffix we can ensure unique matl
			# names (required by Photon-v2 for creating unique materials).
			geometry_name = naming.mangled_geometry_name(b_mesh_object, b_mesh.name, str(material_idx))
			material_name = naming.mangled_material_name(b_mesh_object, b_mesh.name + "_" + b_material.name, str(material_idx))

			# Use the active one as the UV map for export.
			# TODO: support exporting multiple or zero UV maps/layers
			b_uv_layers = b_mesh.uv_layers
			b_active_uv_layer = b_uv_layers.active

			# TODO: support mesh without uv map
			if len(b_mesh.uv_layers) == 0:
				print("warning: mesh (%s) has no uv maps, ignoring" % geometry_name)
				continue

			# TODO: support mesh without uv map
			if b_active_uv_layer is None:
				print("warning: mesh (%s) has %d uv maps, but no one is active (no uv map will be exported)" % (
					geometry_name, len(b_uv_layers)))
				continue

			# TODO: support mesh with multiple uv maps
			if len(b_mesh.uv_layers) > 1:
				print("warning: mesh (%s) has %d uv maps, only the active one is exported" % (
					geometry_name, len(b_uv_layers)))

			triangle_mesh.loop_triangles_to_sdl_triangle_mesh(
				geometry_name,
				self.__sdlconsole,
				loop_triangles,
				b_mesh.vertices,
				b_active_uv_layer.data)

			mat_translate_result = self.exportMaterial(b_context, material_name, b_material)

			# creating actor (can be either model or light depending on emissivity)
			pos, rot, scale = b_mesh_object.matrix_world.decompose()

			if material.is_emissive(b_material):
				light_source_name = naming.mangled_light_source_name(b_mesh_object, b_mesh.name, str(material_idx))
				creator = ModelLightSourceCreator()
				creator.set_data_name(light_source_name)
				creator.set_emitted_radiance(SDLImage(mat_translate_result.surface_emi_res_name))
				creator.set_geometry(SDLGeometry(geometry_name))
				creator.set_material(SDLMaterial(material_name))
				self.get_sdlconsole().queue_command(creator)

				actor_light_name = naming.mangled_actor_light_name(b_mesh_object, "", str(material_idx))
				self.exportActorLight(actor_light_name, light_source_name, geometry_name, material_name, pos, rot, scale)
			else:
				actor_model_name = naming.mangled_actor_model_name(b_mesh_object, "", str(material_idx))
				self.exportActorModel(actor_model_name, geometry_name, material_name, pos, rot, scale)

	def export_camera(self, obj, scene):

		b_camera = obj.data

		if b_camera.type == "PERSP":

			pos, rot, scale = obj.matrix_world.decompose()
			if abs(scale.x - 1.0) > 0.0001 or abs(scale.y - 1.0) > 0.0001 or abs(scale.z - 1.0) > 0.0001:
				print("warning: camera (%s) contains scale factor, ignoring" % b_camera.name)

			# Blender's camera intially pointing (0, 0, -1) with up (0, 1, 0) in its math.py system
			# (also note that Blender's quaternion works this way, does not require q*v*q').
			cam_dir = rot @ mathutils.Vector((0, 0, -1))
			cam_up_dir = rot @ mathutils.Vector((0, 1, 0))
			fov_degrees = 70.0

			lens_unit = b_camera.lens_unit
			if lens_unit == "FOV":
				fov_degrees = math.degrees(b_camera.angle)
			elif lens_unit == "MILLIMETERS":
				sensor_width = b_camera.sensor_width
				focal_length = b_camera.lens
				fov_degrees = math.degrees(math.atan((sensor_width / 2.0) / focal_length)) * 2.0
			else:
				print("warning: camera (%s) with lens unit %s is unsupported, not exporting" % (
					b_camera.name, b_camera.lens_unit))

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
		b_context.view_layer.update()
		b_depsgraph = b_context.evaluated_depsgraph_get()

		# DEBUG
		print(b_depsgraph.mode)
		for b_obj in b_depsgraph.scene.objects:
			if b_obj.type == 'MESH':
				b_mesh = b_obj.data
				for b_material in b_mesh.materials:
					print("original: %s" % b_material.ph_node_tree_name)
					print("evaluated: %s" % b_material.evaluated_get(b_depsgraph).ph_node_tree_name)

		# <objects> contain only objects for display or render.
		for b_obj_instance in b_depsgraph.object_instances:

			# Objects have animations, drivers, modifiers, and constraints applied after being evaluated.
			b_evaluated_obj = b_obj_instance.object

			if b_evaluated_obj.type == "MESH":
				print("exporting mesh " + b_evaluated_obj.name)
				self.export_object_mesh(b_context, b_depsgraph, b_evaluated_obj)
			elif b_evaluated_obj.type == "LIGHT":
				print("exporting light " + b_evaluated_obj.name)
				lights.to_sdl_commands(b_evaluated_obj, self.get_sdlconsole())
			# elif b_evaluated_obj.type == "CAMERA":
			# 	# do nothing since it belongs to core command
			# 	pass
			else:
				print("warning: object (%s) type (%s) is not supported, not exporting" % (
					b_evaluated_obj.name, b_evaluated_obj.type))

		b_world = b_context.scene.world
		if b_world is not None:
			self.export_world(b_world)

	def export(self, b_depsgraph: bpy.types.Depsgraph):
		b_mesh_objects = scene.find_mesh_objects(b_depsgraph)
		b_light_objects = scene.find_light_objects(b_depsgraph)
		b_camera_object = scene.find_active_camera_object(b_depsgraph)

		self.export_object_mesh(b_context, b_depsgraph, b_evaluated_obj)


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

	is_export_animation_requested: BoolProperty(
		name="Export Animation",
		description="Export each frame as a separate scene file.",
		default=False
	)

	def execute(self, b_context):
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

		scene = b_context.scene

		if not self.is_export_animation_requested:

			exporter = Exporter(self.filepath)
			exporter.begin("scene")

			exporter.export_core_commands(b_context)
			exporter.export_world_commands(b_context)

			exporter.end()

			return {'FINISHED'}

		else:

			for frame_number in range(scene.frame_start, scene.frame_end + 1):

				scene.frame_set(frame_number)

				exporter = Exporter(self.filepath)
				exporter.begin("scene_" + str(frame_number).zfill(6))

				exporter.export_core_commands(b_context)
				exporter.export_world_commands(b_context)

				exporter.end()

			return {'FINISHED'}


# Only needed if you want to add into a dynamic menu
def menu_func_export(self, context):
	self.layout.operator(P2Exporter.bl_idname, text="Photon Scene (.p2)")


class ExporterModule(blender.BlenderModule):
	def register(self):
		bpy.utils.register_class(P2Exporter)
		bpy.types.TOPBAR_MT_file_export.append(menu_func_export)

	def unregister(self):
		bpy.types.TOPBAR_MT_file_export.remove(menu_func_export)
		bpy.utils.unregister_class(P2Exporter)


def include_module(module_manager):
	module_manager.add_module(ExporterModule())
