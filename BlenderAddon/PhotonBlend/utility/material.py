"""
@brief General material-related helpers.
"""
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


def find_output_node_from_node_tree(node_tree: bpy.types.NodeTree):
    if node_tree is None:
        return None
    
    # FIXME: PH_OUTPUT is hard-coded; should be resolved after creating its own file
    # if getattr(node, "bl_idname", None) == PhOutputNode.bl_idname:
    output_nodes = [node for node in node_tree.nodes if getattr(node, 'bl_idname', None) == 'PH_OUTPUT']

    if len(output_nodes) > 1:
        print(f"warning: node tree {node_tree.name} contains {len(output_nodes)} output nodes, only the first one is considered")

    return output_nodes[0] if output_nodes else None


def find_output_node_from_material(b_material: bpy.types.Material):
    node_tree = find_node_tree_from_material(b_material)
    return find_output_node_from_node_tree(node_tree)


def is_emissive(b_material: bpy.types.Material):
    output_node = find_output_node_from_material(b_material)
    if output_node is None:
        return False

    # Check whether the surface emission socket is connected
    return output_node.inputs["Surface Emission"].is_linked


def is_masked(b_material: bpy.types.Material):
    output_node = find_output_node_from_material(b_material)
    if output_node is None:
        return False

    # Check whether the surface mask socket is connected
    return output_node.inputs["Surface Mask"].is_linked


def get_emission_image_res_name(b_material: bpy.types.Material):
    """
    @return Name of the SDL resource associated with the emission socket. `None` if `is_emissive()` is `False`.
    """
    output_node = find_output_node_from_material(b_material)
    return output_node.get_surface_emission_res_name(b_material) if output_node is not None else None

def get_mask_image_res_name(b_material: bpy.types.Material):
    """
    @return Name of the SDL resource associated with the mask socket. `None` if `is_masked()` is `False`.
    """
    output_node = find_output_node_from_material(b_material)
    return output_node.get_surface_mask_res_name(b_material) if output_node is not None else None
