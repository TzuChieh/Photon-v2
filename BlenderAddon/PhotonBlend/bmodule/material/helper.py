import bpy


def find_node_tree_from_material(b_material: bpy.types.Material):
    if b_material is None or not b_material.photon.use_nodes:
        return None

    return b_material.photon.node_tree


def find_active_material_from_context(b_context: bpy.types.Context):
    b_object = getattr(b_context, 'active_object', None)
    if b_object is not None and b_object.type not in {'LIGHT', 'CAMERA'}:
        return b_object.active_material

    return None


def find_node_tree_from_context(b_context: bpy.types.Context):
    b_material = find_active_material_from_context(b_context)
    return find_node_tree_from_material(b_material)


def find_output_node_from_node_tree(node_tree):
    if node_tree is None:
        return None

    # TODO: use node name as key?
    for node in node_tree.nodes:
        # FIXME: PH_OUTPUT is hard-coded; should be resolved after creating its own file
        # if getattr(node, "bl_idname", None) == PhOutputNode.bl_idname:
        if getattr(node, "bl_idname", None) == "PH_OUTPUT":
            return node

    return None


def is_emissive(b_material: bpy.types.Material):
    node_tree = find_node_tree_from_material(b_material)
    output_node = find_output_node_from_node_tree(node_tree)
    if output_node is None:
        return False

    # Check whether the surface emission socket is connected
    return output_node.inputs["Surface Emission"].is_linked


def get_emission_image_res_name(b_material: bpy.types.Material):
    node_tree = find_node_tree_from_material(b_material)
    output_node = find_output_node_from_node_tree(node_tree)
    return output_node.get_surface_emi_res_name(b_material)
