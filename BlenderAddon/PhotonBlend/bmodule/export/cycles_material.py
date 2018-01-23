from ... import psdl
from ... import utility

import bpy
import mathutils

from collections import namedtuple


class TranslateResult:

	def __init__(self, command = None, sdlri = None, is_emissive = False):
		self.command     = command
		self.sdlri       = sdlri
		self.is_emissive = is_emissive

	def is_valid(self):
		return self.command is not None or self.sdlri is not None


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
		return image_sdlri

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

		if color_socket.links[0].from_node.name == "Image Texture":
			image_texture_node = color_socket.links[0].from_node
			image_sdlri = translate_image_texture_node(image_texture_node, sdlconsole, res_name)
			if image_sdlri.is_valid():
				command.set_albedo_image(image_sdlri)
			else:
				print("warning: material %s's albedo image is invalid (identifier = %s)" % (res_name, image_sdlri))
			sdlconsole.queue_command(command)

			return TranslateResult(command)

		else:
			print("warning: cannot handle Diffuse BSDF node's color socket (material %s)" % res_name)

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


NODE_NAME_TO_TRANSLATOR_TABLE = {
	"Diffuse BSDF": translate_diffuse_bsdf_node,
	"Glossy BSDF":  translate_glossy_bsdf_node
}


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

