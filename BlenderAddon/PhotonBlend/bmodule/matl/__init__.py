import bpy
from ..node import PhOutputNode


def find_node_tree(b_material: bpy.types.Material):
	if b_material is None or b_material.ph_node_tree_name == "":
		return None

	return bpy.data.node_groups[b_material.ph_node_tree_name]


def find_output_node(node_tree):
	if node_tree is None:
		return None

	for node in node_tree.nodes:
		if getattr(node, "bl_idname", None) == PhOutputNode.bl_idname:
			return node

	return None


def is_emissive(b_material: bpy.types.Material):
	output_node = find_output_node(find_node_tree(b_material))
	if output_node is None:
		return False

	# Check whether the surface emission socket is connected
	return output_node.inputs["Surface Emission"].is_linked
