from ... import psdl

import bpy
import mathutils

from collections import namedtuple


def non_node_material_to_sdl(b_material, res_name):

	print("warning: material %s uses no nodes, exporting diffuse color only" % res_name)

	diffuse       = b_material.diffuse_color
	diffuse_color = mathutils.Color((diffuse[0], diffuse[1], diffuse[2]))

	command = psdl.materialcmd.MatteOpaqueCreator()
	command.set_data_name(res_name)
	command.set_albedo(diffuse_color)

	return command.to_sdl()


#def image_texture_node_to_sdl_resource(this_node, res_name):

	#sdl_resource = namedtuple(sdl_resource, ["res_name", "command"])




def diffuse_bsdf_node_to_sdl(this_node, res_name):

	color_socket = this_node.inputs.get("Color")
	if color_socket.is_linked:

		if color_socket.links[0].from_node == "Image Texture":
			image_texture_node = color_socket.links[0].from_node
			#image_texture_node_to_sdl(image_texture_node, )

			# TODO

		else:
			print("warning: cannot handle Diffuse BSDF node's color socket (material %s)" % res_name)

	# TODO: color has 4 components, currently parsing 3 only
	color = color_socket.default_value

	# TODO: handle roughness & normal sockets

	albedo = mathutils.Color((color[0], color[1], color[2]))

	command = psdl.materialcmd.MatteOpaqueCreator()
	command.set_data_name(res_name)
	command.set_albedo(albedo)

	return command.to_sdl()


def glossy_bsdf_node_to_sdl(this_node, res_name):

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

		return command.to_sdl()

	else:
		print("warning: cannot convert Glossy BSDF distribution type %s (material %s)" %
			  (distribution, res_name))
		return ""


NODE_NAME_TO_PSDL_TRANSLATOR_TABLE = {
	"Diffuse BSDF": diffuse_bsdf_node_to_sdl,
	"Glossy BSDF" : glossy_bsdf_node_to_sdl
}


def surface_node_to_sdl(this_node, res_name):

	translator = NODE_NAME_TO_PSDL_TRANSLATOR_TABLE.get(this_node.name)
	if translator is not None:
		return translator(this_node, res_name)
	else:
		print("warning: material %s has no valid psdl translator, ignoring" % res_name)
		return ""


def node_material_to_sdl(b_material, res_name):

	material_output_node = b_material.node_tree.nodes.get("Material Output")
	if material_output_node is not None:
		surface_socket = material_output_node.inputs.get("Surface")

		if surface_socket.is_linked:
			surface_node = surface_socket.links[0].from_node
			return surface_node_to_sdl(surface_node, res_name)
		else:
			print("warning: materia %s has no linked surface node, ignoring" % res_name)
			return ""
	else:
		print("warning: material %s has no output node, ignoring" % res_name)
		return ""


def to_sdl(b_material, res_name):

	if b_material.use_nodes:
		return node_material_to_sdl(b_material, res_name)
	else:
		return non_node_material_to_sdl(b_material, res_name)

