import bpy


def find_objects_of_type(b_depsgraph: bpy.types.Depsgraph, b_object_type):
	return [b_object for b_object in b_depsgraph.objects if b_object.type == b_object_type]


def find_mesh_objects(b_depsgraph: bpy.types.Depsgraph):
	return find_objects_of_type(b_depsgraph, 'MESH')


def find_light_objects(b_depsgraph: bpy.types.Depsgraph):
	return find_objects_of_type(b_depsgraph, 'LIGHT')


def find_camera_objects(b_depsgraph: bpy.types.Depsgraph):
	return find_objects_of_type(b_depsgraph, 'CAMERA')


def find_active_camera_object(b_depsgraph: bpy.types.Depsgraph):
	return b_depsgraph.scene_eval.camera


def find_materials_from_mesh_objects(b_mesh_objects, b_depsgraph: bpy.types.Depsgraph):
	"""
	Get a list of unique materials.
	@param b_mesh_objects List of mesh objects to find materials from.
	"""
	name_to_material = {}
	for b_object in b_mesh_objects:
		b_mesh = b_object.data
		for b_material in b_mesh.materials:
			# Could be `None`, e.g., a material slot with no material assigned
			if not b_material:
				continue

			# Get the evaluated material if the mesh data block is evaluated
			if b_mesh.is_evaluated:
				b_material = b_material.evaluated_get(b_depsgraph)
			
			# TODO: materials from other blend files (library blends) can have the same name, need to somehow distinguish them (ID.name_full?)
			name_to_material[b_material.name] = b_material

	return name_to_material.values()
