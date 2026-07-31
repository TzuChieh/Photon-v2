import bpy


def find_objs_of_type(b_depsgraph: bpy.types.Depsgraph, b_obj_type):
	return [b_obj for b_obj in b_depsgraph.objects if b_obj.type == b_obj_type]


def iter_mesh_obj_instances(b_depsgraph: bpy.types.Depsgraph):
	"""
	Iterate visible evaluated mesh object instances.

	Do not keep `b_obj_instance` or `b_obj_instance.object` between iterations. Copy values such as
	`b_obj_instance.matrix_world` if needed later. Evaluated data-blocks such as
	`b_obj_instance.object.data` may be kept after the loop, but only until the dependency graph is
	reevaluated.
	"""
	for depsgraph_index, b_obj_instance in enumerate(b_depsgraph.object_instances):
		if b_obj_instance.object.type != 'MESH' or not b_obj_instance.show_self:
			continue

		yield depsgraph_index, b_obj_instance


def find_materials_from_mesh_obj_instances(b_depsgraph: bpy.types.Depsgraph):
	"""
	Get unique materials used by visible evaluated mesh object instances.
	"""
	b_materials_by_name = {}
	for _, b_obj_instance in iter_mesh_obj_instances(b_depsgraph):
		b_mesh_obj = b_obj_instance.object
		for b_material_slot in b_mesh_obj.material_slots:
			b_material = b_material_slot.material
			# Could be `None`, e.g., a material slot with no material assigned
			if not b_material:
				continue

			# Use the evaluated material referenced by an evaluated mesh
			if b_mesh_obj.data.is_evaluated:
				b_material = b_material.evaluated_get(b_depsgraph)

			# TODO: Materials from other blend files can have the same name. Find a way to
			# distinguish them (ID.name_full?).
			b_materials_by_name[b_material.name] = b_material

	return list(b_materials_by_name.values())


def find_mesh_objs(b_depsgraph: bpy.types.Depsgraph):
	return find_objs_of_type(b_depsgraph, 'MESH')


def find_light_objs(b_depsgraph: bpy.types.Depsgraph):
	return find_objs_of_type(b_depsgraph, 'LIGHT')


def find_camera_objs(b_depsgraph: bpy.types.Depsgraph):
	return find_objs_of_type(b_depsgraph, 'CAMERA')


def find_active_camera_obj(b_depsgraph: bpy.types.Depsgraph):
	return b_depsgraph.scene_eval.camera
