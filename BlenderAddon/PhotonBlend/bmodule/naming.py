"""
Utilities for deriving stable and unique SDL resource names from Blender data.

Names use the original Blender ID's session UID on Blender 4.5+. Blender 3.6 uses the
library-qualified ID name. Suffixes decorate resources further for situations where
the default naming is insufficient.
"""
import bpy

# TODO: materials from other blend files (library blends) can have the same name, need to somehow distinguish them (ID.name_full?)


def join_name_parts(*parts):
	return "_".join(str(part) for part in parts if part not in (None, ""))


def _get_mangled_id_name(prefix, b_id: bpy.types.ID, *suffixes):
	b_original_id = b_id.original
	if bpy.app.version >= (4, 5, 0):
		id_name = b_original_id.session_uid
	else:
		id_name = b_original_id.name_full

	return join_name_parts(prefix, id_name, *suffixes)


def get_mangled_mesh_name(b_mesh_obj: bpy.types.Object, *suffixes):
	"""
	Get a unique mesh resource name for a Blender mesh object.
	"""
	assert isinstance(b_mesh_obj.data, bpy.types.Mesh)

	# Object modifiers can change the geometry, so use object identity.
	return _get_mangled_id_name("Mesh", b_mesh_obj, *suffixes)


def get_mangled_material_name(b_material: bpy.types.Material, *suffixes):
	"""
	Get a unique resource name for the material resource.
	"""
	return _get_mangled_id_name("Material", b_material, *suffixes)


def get_mangled_light_name(b_light_obj: bpy.types.Object, *suffixes):
	"""
	Get a unique light resource name for a Blender light object.
	"""
	assert isinstance(b_light_obj.data, bpy.types.Light)

	return _get_mangled_id_name("Light", b_light_obj, *suffixes)


def get_mangled_camera_name(b_camera_obj: bpy.types.Object, *suffixes):
	"""
	Get a unique camera resource name for a Blender camera object.
	"""
	assert isinstance(b_camera_obj.data, bpy.types.Camera)

	return _get_mangled_id_name("Camera", b_camera_obj, *suffixes)


def get_mangled_world_name(b_world: bpy.types.World, *suffixes):
	"""
	Get a unique resource name for the world resource.
	"""
	return _get_mangled_id_name("World", b_world, *suffixes)


def get_mangled_actor_name(b_object: bpy.types.Object, *suffixes):
	"""
	Get a unique actor resource name for a Blender object.
	"""
	return _get_mangled_id_name("Object", b_object, *suffixes)


def get_mangled_node_name(b_node: bpy.types.Node, b_material: bpy.types.Material, *suffixes):
	"""
	Get a unique resource name for the node resource.
	"""
	# Material identity is required since node names are unique within the same node tree only.
	return _get_mangled_id_name("Node", b_material, b_node.name, *suffixes)


def _get_mangled_node_socket_name(
	prefix,
	b_node_socket: bpy.types.NodeSocket,
	b_material: bpy.types.Material,
	*suffixes):
	# Note that the identifier attribute of a socket is only unique in either input or output sockets, not both.
	b_owning_node = b_node_socket.node
	return _get_mangled_id_name(
		prefix,
		b_material,
		b_owning_node.name,
		b_node_socket.identifier,
		*suffixes)


def get_mangled_input_node_socket_name(
	b_node_socket: bpy.types.NodeSocket,
	b_material: bpy.types.Material,
	*suffixes):

	return _get_mangled_node_socket_name("InputNodeSocket", b_node_socket, b_material, *suffixes)


def get_mangled_output_node_socket_name(
	b_node_socket: bpy.types.NodeSocket,
	b_material: bpy.types.Material,
	*suffixes):

	return _get_mangled_node_socket_name("OutputNodeSocket", b_node_socket, b_material, *suffixes)
