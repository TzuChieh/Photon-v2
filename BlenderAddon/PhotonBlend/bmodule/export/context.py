from dataclasses import dataclass

from bmodule.mesh import attributes


@dataclass
class ExportContext:
    # Maps Blender geometry attributes to Photon custom attribute slots.
    geometry_attribute_to_custom_slot: dict[attributes.GeometryAttribute, int]
