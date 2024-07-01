"""
@brief Additional world data block properties.
"""
from utility import blender

import bpy


class PhotonWorldData(bpy.types.PropertyGroup):
    background_type: bpy.props.EnumProperty(
        items=[
            ('NONE', "None", "", 0),
            ('IMAGE', "Image", "", 1),
            ('PREETHAM', "Preetham", "", 2),
        ],
        name="Background Type",
        description="Type of the background.",
        default='NONE'
	)

    up_rotation: bpy.props.FloatProperty(
        name="Up Rotation",
        description="Up-axis rotation in degrees.",
        default=0,
        min=0,
        max=360
    )

    image_file_path: bpy.props.StringProperty(
        name="File",
        default="",
        subtype='FILE_PATH'
    )

    preetham_turbidity: bpy.props.FloatProperty(
        name="Turbidity",
        description="",
        default=2,
        min=1,
        max=64
    )

    standard_time: bpy.props.FloatProperty(
        name="Standard Time",
        description="Standard time in 24H.",
        default=12,
        min=0,
        max=24
    )

    standard_meridian: bpy.props.FloatProperty(
        name="Standard Meridian",
        description="Standard meridian in degrees.",
        default=0,
        min=-180,
        max=180
    )

    latitude: bpy.props.FloatProperty(
        name="Latitude",
        description="Site latitude.",
        default=0,
        min=-90,
        max=90
    )

    longitude: bpy.props.FloatProperty(
        name="Longitude",
        description="Site longitude.",
        default=0,
        min=-180,
        max=180
    )

    julian_date: bpy.props.IntProperty(
        name="Julian Date",
        description="Julian date.",
        default=1,
        min=1,
        max=366
    )

    energy_scale: bpy.props.FloatProperty(
        name="Energy Scale",
        description="A non-physical scale factor for artistic purpose.",
        default=1.0
    )

@blender.register_module
class WorldProperties(blender.BlenderModule):
    def register(self):
        bpy.utils.register_class(PhotonWorldData)

        bpy.types.World.photon = bpy.props.PointerProperty(
            type=PhotonWorldData,
            name="Photon World Data")

    def unregister(self):
        bpy.utils.unregister_class(PhotonWorldData)
