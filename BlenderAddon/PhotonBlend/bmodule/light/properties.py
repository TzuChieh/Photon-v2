"""
@brief Additional light data block properties.
"""
from utility import blender
from psdl import sdl

import bpy

import sys


class PhotonLightData(bpy.types.PropertyGroup):
    color_linear_srgb: bpy.props.FloatVectorProperty(
        name="Color",
        description="light color in linear sRGB",
        default=[1.0, 1.0, 1.0],
        min=0.0,
        max=1.0,
        subtype="COLOR",
        size=3
    )

    watts: bpy.props.FloatProperty(
        name="Watts",
        description="light energy in watts",
        default=100,
        min=0.0,
        max=sys.float_info.max
    )

    attenuation_type: bpy.props.EnumProperty(
		items=[
			('NONE', "None", "", 0),
			('IES', "IES", "", 1)
		],
		name="Attenuation",
		description="Type of the lighting attenuation.",
		default='NONE'
	)

    ies_file_path : bpy.props.StringProperty(
		name="File",
		default="",
		subtype='FILE_PATH'
	)

    directly_visible: bpy.props.BoolProperty(
        name="Directly Visible",
        description=sdl.GeometricLightActorCreator.set_directly_visible.__doc__,
        default=True
    )

    bsdf_sample: bpy.props.BoolProperty(
        name="BSDF Sample Technique",
        description=sdl.GeometricLightActorCreator.set_bsdf_sample.__doc__,
        default=True
    )

    point_light_bsdf_sample: bpy.props.BoolProperty(
        name="BSDF Sample Technique (Point Light)",
        description=sdl.GeometricLightActorCreator.set_bsdf_sample.__doc__,
        default=False
    )

    direct_sample: bpy.props.BoolProperty(
        name="Direct Sample Technique",
        description=sdl.GeometricLightActorCreator.set_direct_sample.__doc__,
        default=True
    )

    emission_sample: bpy.props.BoolProperty(
        name="Emission Sample Technique",
        description=sdl.GeometricLightActorCreator.set_emission_sample.__doc__,
        default=True
    )


@blender.register_module
class LightProperties(blender.BlenderModule):
    def register(self):
        bpy.utils.register_class(PhotonLightData)

        bpy.types.Light.photon = bpy.props.PointerProperty(
            type=PhotonLightData,
            name="Photon Light Data")

    def unregister(self):
        bpy.utils.unregister_class(PhotonLightData)
