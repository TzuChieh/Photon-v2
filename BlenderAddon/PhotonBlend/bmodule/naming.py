"""
Utilities for providing unique names (e.g., through name mangling or other means) for Photon resources in Blender.
Uniqueness of a name is guaranteed across all supported resource base categories. For example, two object resources will
never have the same mangled names, and a mesh resource will never have the same mangled name as a material resource.

In situations where name conflict is possible, they are expected to be resolved via the specified decorations.
"""

import bpy

# TODO: materials from other blend files (library blends) can have the same name, need to somehow distinguish them (ID.name_full?)


def _get_decorated_name(name, **decorations):
	"""
	Decorates a name given predefined decoration elements.
	@param **decorations `prefix`: string to append in the front of `name`; `suffix`: string to append in the back of `name`
	"""
	decorated_name = name
	if 'prefix' in decorations:
		decorated_name = decorations.get('prefix') + "_" + decorated_name
	if 'suffix' in decorations:
		decorated_name = decorated_name + "_" + decorations.get('suffix')

	return decorated_name


def get_mangled_mesh_name(b_mesh: bpy.types.Mesh, **decorations):
	"""
	Get a unique resource name for the mesh resource.
	"""
	return "Mesh_" + _get_decorated_name(b_mesh.name, **decorations)


def get_mangled_material_name(b_material: bpy.types.Material, **decorations):
	"""
	Get a unique resource name for the material resource.
	"""
	return "Material_" + _get_decorated_name(b_material.name, **decorations)


# Get a unique resource name for the light resource.
def get_mangled_light_name(b_light: bpy.types.Light, **decorations):
	"""
	Get a unique resource name for the light resource.
	"""
	return "Light_" + _get_decorated_name(b_light.name, **decorations)


def get_mangled_object_name(b_object: bpy.types.Object, **decorations):
	"""
	Get a unique resource name for the object resource.
	"""
	return "Object_" + _get_decorated_name(b_object.name, **decorations)


def get_mangled_node_name(b_node: bpy.types.Node, b_material: bpy.types.Material, **decorations):
	"""
	Get a unique resource name for the node resource.
	"""
	# Material name is required since node name is unique within the same node tree only.
	joint_name = _get_decorated_name(b_node.name, prefix=b_material.name)

	return "Node_" + _get_decorated_name(joint_name, **decorations)


def _get_mangled_node_socket_name(
	b_node_socket: bpy.types.NodeSocket,
	b_material: bpy.types.Material,
	**decorations):
	# Note that the identifier attribute of a socket is only unique in either input or output sockets, not both.
	b_owning_node = b_node_socket.node
	joint_name = _get_decorated_name(b_node_socket.identifier, prefix=b_owning_node.name)
	joint_name = _get_decorated_name(joint_name, prefix=b_material.name)

	return _get_decorated_name(joint_name, **decorations)


def get_mangled_input_node_socket_name(
	b_node_socket: bpy.types.NodeSocket,
	b_material: bpy.types.Material,
	**decorations):

	return "InputNode_" + _get_mangled_node_socket_name(b_node_socket, b_material, **decorations)


def get_mangled_output_node_socket_name(
	b_node_socket: bpy.types.NodeSocket,
	b_material: bpy.types.Material,
	**decorations):

	return "OutputNode_" + _get_mangled_node_socket_name(b_node_socket, b_material, **decorations)
