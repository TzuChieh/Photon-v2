from enum import Enum
import random

from utility import material

from bpy_extras import mesh_utils
import numpy as np


# Must not exceed max custom slots supported by `EPrimitiveAttribute`
_MAX_CUSTOM_ATTRIBUTES = 4


class GeometryAttribute(Enum):
    # Blender Geometry node's Random Per Island value, shared by each connected mesh island.
    RANDOM_PER_ISLAND = 'random-per-island'


def find_used_geometry_attributes(b_materials):
    """
    @return A set of `GeometryAttribute` that is being used by `b_materials`.
    """
    used_geometry_attributes = set()
    for b_material in b_materials:
        if b_material is None:
            continue

        for b_node in material.find_reachable_nodes_from_material(b_material):
            used_geometry_attributes.update(b_node.used_geometry_attributes())

    return used_geometry_attributes


def assign_custom_slots(geometry_attributes):
    """
    @return A dense mapping from used `GeometryAttribute` values to Photon custom attribute slots.
    Slots are assigned deterministically in `GeometryAttribute` declaration order.
    """
    slot_ordered_attributes = tuple(attr for attr in GeometryAttribute if attr in geometry_attributes)
    if len(slot_ordered_attributes) > _MAX_CUSTOM_ATTRIBUTES:
        raise ValueError(
            f"scene requires {len(slot_ordered_attributes)} geometry attributes, "
            f"but Photon supports {_MAX_CUSTOM_ATTRIBUTES}")

    return {attr: slot for slot, attr in enumerate(slot_ordered_attributes)}


def calc_random_per_island(b_mesh):
    """
    Generate deterministic face-domain values resembling Blender's Random Per Island.
    @return Array of per-island random values, indexed by loop triangle indices.
    """
    # A fixed seed keeps the values stable across exports
    random_generator = random.Random(0)
    values = np.empty(len(b_mesh.loop_triangles), dtype=np.float32)
    for triangle_island in mesh_utils.mesh_linked_triangles(b_mesh):
        island_value = random_generator.random()
        for triangle in triangle_island:
            values[triangle.index] = island_value

    return values
