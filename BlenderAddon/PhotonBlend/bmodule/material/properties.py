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


def register():
    bpy.utils.register_class(PhotonMaterialData)

    bpy.types.Material.photon = bpy.props.PointerProperty(
        type=PhotonMaterialData,
        name="Photon Material Data")


def unregister():
    bpy.utils.unregister_class(PhotonMaterialData)
