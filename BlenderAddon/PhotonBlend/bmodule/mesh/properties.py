"""
@brief Additional mesh data block properties.
"""
from utility import blender

import bpy


class PhotonMeshData(bpy.types.PropertyGroup):
    export_type: bpy.props.EnumProperty(
        items=[
            ('ORIGINAL', "Original", "As the original appearance in Blender.", 0),
            ('MENGER_SPONGE', "Menger Sponge", "As a menger sponge.", 1),
        ],
        name="Export As",
        description="How the mesh data is exported, e.g., one can keep the transforms and export it as another type.",
        default='ORIGINAL'
    )


@blender.register_module
class MeshProperties(blender.BlenderModule):
    def register(self):
        bpy.utils.register_class(PhotonMeshData)

        bpy.types.Mesh.photon = bpy.props.PointerProperty(
            type=PhotonMeshData,
            name="Photon Mesh Data")

    def unregister(self):
        bpy.utils.unregister_class(PhotonMeshData)
