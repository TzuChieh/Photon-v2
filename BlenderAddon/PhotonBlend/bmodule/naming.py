"""
Utilities for providing unique names (e.g., through name mangling or other means) for Photon resources in Blender.
Uniqueness of a name is guaranteed across all supported resource base categories. For example, two object resources will
never have the same mangled names, and a mesh resource will never have the same mangled name as a material resource.

In situations where name conflict is possible, they are expected to be resolved via the specified decorations.
"""

import bpy

# TODO: materials from other blend files (library blends) can have the same name, need to somehow distinguish them


# Decorates a name given predefined decoration elements.
def _get_decorated_name(name, **decorations):
	decorated_name = name
	if 'prefix' in decorations:
		decorated_name = decorations.get('prefix') + "$" + decorated_name
	if 'suffix' in decorations:
		decorated_name = decorated_name + "$" + decorations.get('suffix')

	return decorated_name


# Get a new resource name from an existing name.
def get_mangled_name(src_name, **decorations):
	return _get_decorated_name(src_name, **decorations)


# Get a unique resource name for the mesh resource.
def get_mangled_mesh_name(b_mesh: bpy.types.Mesh, **decorations):
	return "GE" + _get_decorated_name(b_mesh.name, **decorations)


# Get a unique resource name for the material resource.
def get_mangled_material_name(b_material: bpy.types.Material, **decorations):
	return "MA" + _get_decorated_name(b_material.name, **decorations)


# Get a unique resource name for the light resource.
def get_mangled_light_name(b_light: bpy.types.Light, **decorations):
	return "LI" + _get_decorated_name(b_light.name, **decorations)


# Get a unique resource name for the object resource.
def get_mangled_object_name(b_object: bpy.types.Object, **decorations):
	return "OB" + _get_decorated_name(b_object.name, **decorations)


# Get a unique resource name for the node resource.
def get_mangled_node_name(b_node: bpy.types.Node, b_material: bpy.types.Material, **decorations):
	# Material name is required since node name is unique within the same node tree only.
	joint_name = _get_decorated_name(b_node.name, prefix=b_material.name)

	return "NO" + _get_decorated_name(joint_name, **decorations)


# Note that the identifier attribute of a socket is only unique in either input or output sockets, not both.
def _get_mangled_node_socket_name(
	b_node_socket: bpy.types.NodeSocket,
	b_material: bpy.types.Material,
	**decorations):

	b_owning_node = b_node_socket.node
	joint_name = _get_decorated_name(b_node_socket.identifier, prefix=b_owning_node.name)
	joint_name = _get_decorated_name(joint_name, prefix=b_material.name)

	return _get_decorated_name(joint_name, **decorations)


def get_mangled_input_node_socket_name(
	b_node_socket: bpy.types.NodeSocket,
	b_material: bpy.types.Material,
	**decorations):

	return "IN" + _get_mangled_node_socket_name(b_node_socket, b_material, **decorations)


def get_mangled_output_node_socket_name(
	b_node_socket: bpy.types.NodeSocket,
	b_material: bpy.types.Material,
	**decorations):

	return "OU" + _get_mangled_node_socket_name(b_node_socket, b_material, **decorations)
