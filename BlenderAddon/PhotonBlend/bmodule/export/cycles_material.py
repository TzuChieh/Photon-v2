from ... import psdl

import bpy
import mathutils


def non_node_material_to_sdl(b_material, res_name):

	print("warning: material %s uses no nodes, exporting diffuse color only" % res_name)

	diffuse       = b_material.diffuse_color
	diffuse_color = mathutils.Color((diffuse[0], diffuse[1], diffuse[2]))

	command = psdl.materialcmd.MatteOpaque.create(res_name, diffuse_color)
	return command.to_sdl()


def node_material_to_sdl(b_material, res_name):

	# TODO: convert cycles node-based materials
	print("material %s exporting ignored" % res_name)
	return ""


def to_sdl(b_material, res_name):

	if b_material.use_nodes:
		return node_material_to_sdl(b_material, res_name)
	else:
		return non_node_material_to_sdl(b_material, res_name)

