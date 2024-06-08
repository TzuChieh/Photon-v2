"""
@brief Additional material data block properties.
"""
from utility import blender

import bpy


class PhotonMaterialData(bpy.types.PropertyGroup):
    use_nodes: bpy.props.BoolProperty(
        name="Use Node Tree",
        description="Use Photon's material nodes or not.",
        default=True
    )

    node_tree: bpy.props.PointerProperty(
        type=bpy.types.NodeTree,
        name="Node Tree"
    )


@blender.register_module
class MaterialProperties(blender.BlenderModule):
    def register(self):
        bpy.utils.register_class(PhotonMaterialData)

        bpy.types.Material.photon = bpy.props.PointerProperty(
            type=PhotonMaterialData,
            name="Photon Material Data")

    def unregister(self):
        bpy.utils.unregister_class(PhotonMaterialData)
