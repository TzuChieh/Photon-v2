from ... import psdl
from ... import utility

import bpy
import mathutils

from collections import namedtuple


class TranslateResult:

	def __init__(self,
	             sdl_command               = None,
	             sdl_resource_identifier   = None,
	             sdl_emission_image_command = None):
		self.sdl_command                = sdl_command
		self.sdl_resource_identifier    = sdl_resource_identifier
		self.sdl_emission_image_command = sdl_emission_image_command

	def is_valid(self):
		return (self.sdl_command is not None or
		        self.sdl_resource_identifier is not None or
		        self.is_emissive())

	def is_emissive(self):
		return self.sdl_emission_image_command is not None


def translate_non_node_material(b_material, sdlconsole, res_name):

	print("warning: material %s uses no nodes, exporting diffuse color only" % res_name)

	diffuse       = b_material.diffuse_color
	diffuse_color = mathutils.Color((diffuse[0], diffuse[1], diffuse[2]))

	command = psdl.materialcmd.MatteOpaqueCreator()
	command.set_data_name(res_name)
	command.set_albedo_color(diffuse_color)
	sdlconsole.queue_command(command)

	return TranslateResult(command)


def translate_image_texture_node(this_node, sdlconsole, res_name):

	image       = this_node.image
	image_sdlri = psdl.sdlresource.SdlResourceIdentifier()
	if image is None:
		return TranslateResult()

	image_name = utility.get_filename_without_ext(image.name)
	image_sdlri.append_folder(res_name)
	image_sdlri.set_file(image_name + ".png")
	image.file_format = "PNG"
	image.alpha_mode  = "PREMUL"
	psdl.sdlresource.save_blender_image(image, image_sdlri, sdlconsole)

	return TranslateResult(None, image_sdlri)


def translate_diffuse_bsdf_node(this_node, sdlconsole, res_name):

	command = psdl.materialcmd.MatteOpaqueCreator()
	command.set_data_name(res_name)

	color_socket = this_node.inputs.get("Color")
	if color_socket.is_linked:

		result = translate_node(color_socket.links[0].from_node, sdlconsole, res_name)
		if result.sdl_resource_identifier is not None:
			command.set_albedo_image_sdlri(result.sdl_resource_identifier)
		else:
			print("warning: material %s's albedo image is invalid" % res_name)
		sdlconsole.queue_command(command)
		return TranslateResult(command)

	# TODO: color has 4 components, currently parsing 3 only
	color = color_socket.default_value

	# TODO: handle roughness & normal sockets

	albedo = mathutils.Color((color[0], color[1], color[2]))
	command.set_albedo_color(albedo)
	sdlconsole.queue_command(command)

	return TranslateResult(command)


def translate_glossy_bsdf_node(this_node, sdlconsole, res_name):

	distribution = this_node.distribution
	if distribution == "GGX":

		roughness_socket = this_node.inputs[1]
		roughness        = 0.5
		if not roughness_socket.is_linked:
			roughness = roughness_socket.default_value
		else:
			print("warning: cannot handle non-leaf Glossy BSDF node (material %s)" % res_name)

		color_socket = this_node.inputs[0]
		color        = (0.5, 0.5, 0.5, 0.5)
		if not color_socket.is_linked:
			color = color_socket.default_value
		else:
			print("warning: cannot handle non-leaf Glossy BSDF node (material %s)" % res_name)

		command = psdl.materialcmd.AbradedOpaqueCreator()
		command.set_data_name(res_name)
		command.set_albedo(mathutils.Color((0, 0, 0)))
		command.set_f0(mathutils.Color((color[0], color[1], color[2])))
		command.set_roughness(roughness)
		command.set_anisotropicity(False)
		sdlconsole.queue_command(command)

		return TranslateResult(command)

	else:
		print("warning: cannot convert Glossy BSDF distribution type %s (material %s)" %
		      (distribution, res_name))
		return TranslateResult()


def translate_vector_math_node(node, sdlconsole, res_name):

	vector_socket_0       = node.inputs[0]
	operand_image_command = None
	if vector_socket_0.is_linked:
		result_0 = translate_node(vector_socket_0.links[0].from_node, sdlconsole, res_name)
		if result_0.sdl_command is not None:
			operand_image_command = result_0.sdl_command
		else:
			print("material %s's vector math node operand image conversion failed, using default value" % res_name)

	if operand_image_command is None:
		operand_image_command = psdl.imagecmd.ConstantImageCreator()
		operand_image_command.set_data_name("socket_0_value_" + res_name)  # FIXME: be aware of name collision
		socket0_vec3 = vector_socket_0.inputs[0].default_value
		operand_image_command.set_vec3_value(socket0_vec3)
		sdlconsole.queue_command(operand_image_command)

	vector_socket_1 = node.inputs[1]
	if vector_socket_1.is_linked:
		print("cannot handle linked socket 1 in vector math node (material %s)" % res_name)

	socket1_vec3 = vector_socket_1.inputs[0].default_value
	print("averaging socket 1 vector components (material %s)" % res_name)
	socket1_value = (socket1_vec3.x + socket1_vec3.y + socket1_vec3.z) / 3.0

	math_image_command = psdl.imagecmd.RealMathImageCreator()
	math_image_command.set_data_name("vector_math_" + res_name)  # FIXME: be aware of name collision
	math_image_command.set_real_value(socket1_value)
	math_image_command.set_operand_image(operand_image_command.get_data_name())

	if node.operation == "ADD":
		math_image_command.set_add()
	elif node.operation == "SUBTRACT":
		math_image_command.set_add()
		math_image_command.set_real_value(-socket1_value)
	else:
		print("unsupported operation %s in vector math node of material %s" % (node.operation, res_name))

	sdlconsole.queue_command(math_image_command)
	return TranslateResult(math_image_command)


def translate_emission_node(this_node, sdlconsole, res_name):

	color_socket  = this_node.inputs.get("Color")
	image_command = None
	if color_socket.is_linked:

		color_socket_from_node = color_socket.links[0].from_node
		if color_socket_from_node.name == "Image Texture":

			image_texture_node = color_socket_from_node
			result             = translate_image_texture_node(image_texture_node, sdlconsole, res_name)
			if result.sdl_resource_identifier is not None:
				image_command = psdl.imagecmd.LdrPictureImageCreator()
				image_command.set_data_name("emission_image_" + res_name)  # FIXME: be aware of name collision
				image_command.set_image_sdlri(result.sdl_resource_identifier)
			else:
				print("warning: material %s's emission image is invalid" % res_name)

	if image_command is None:
		image_command = psdl.imagecmd.ConstantImageCreator()
		image_command.set_data_name("emission_default_value_" + res_name)  # FIXME: be aware of name collision
		color_array = color_socket.default_value
		image_command.set_rgb_value(mathutils.Color((color_array[0], color_array[1], color_array[2])))

	sdlconsole.queue_command(image_command)

	strength = this_node.inputs[1].default_value
	if strength != 1.0:
		math_image = psdl.imagecmd.RealMathImageCreator()
		math_image.set_data_name("emission_strength_modifier_" + res_name)  # FIXME: be aware of name collision
		math_image.set_multiply()
		math_image.set_real_value(strength)
		math_image.set_operand_image(image_command.get_data_name())
		sdlconsole.queue_command(math_image)
		image_command = math_image

	return TranslateResult(None, None, image_command)


NODE_NAME_TO_TRANSLATOR_TABLE = {
	"Diffuse BSDF":  translate_diffuse_bsdf_node,
	"Glossy BSDF":   translate_glossy_bsdf_node,
	"Emission":      translate_emission_node,
	"Image Texture": translate_image_texture_node,
	"Vector Math":   translate_vector_math_node
}


def translate_node(node, sdlconsole, res_name):

	translator = NODE_NAME_TO_TRANSLATOR_TABLE.get(node.name)
	if translator is not None:
		return translator(node, sdlconsole, res_name)
	else:
		print("warning: no valid psdl translator for node %s (material %s)" % (node.name, res_name))
		return TranslateResult()


def translate_surface_node(this_node, sdlconsole, res_name):

	translator = NODE_NAME_TO_TRANSLATOR_TABLE.get(this_node.name)
	if translator is not None:
		return translator(this_node, sdlconsole, res_name)
	else:
		print("warning: material %s has no valid psdl translator, ignoring" % res_name)
		return TranslateResult()


def translate_node_material(b_material, sdlconsole, res_name):

	material_output_node = b_material.node_tree.nodes.get("Material Output")
	if material_output_node is not None:
		surface_socket = material_output_node.inputs.get("Surface")

		if surface_socket.is_linked:
			surface_node = surface_socket.links[0].from_node
			return translate_surface_node(surface_node, sdlconsole, res_name)
		else:
			print("warning: materia %s has no linked surface node, ignoring" % res_name)
			return TranslateResult()
	else:
		print("warning: material %s has no output node, ignoring" % res_name)
		return TranslateResult()


def translate(b_material, sdlconsole, res_name):

	if b_material.use_nodes:
		return translate_node_material(b_material, sdlconsole, res_name)
	else:
		return translate_non_node_material(b_material, sdlconsole, res_name)

