from ...psdl.sdlconsole import SdlConsole
from ...psdl.pysdl import PointLightSourceCreator
from ...psdl.pysdl import SDLReal, SDLVector3
from .. import naming

import bpy


def light_to_sdl_point_light(b_light: bpy.types.PointLight, console: SdlConsole):
    source_name = naming.get_mangled_light_name(b_light)

    creator = PointLightSourceCreator()
    creator.set_data_name(source_name)
    creator.set_linear_srgb(SDLVector3(b_light.ph_light_color_linear_srgb))
    creator.set_watts(SDLReal(b_light.ph_light_watts))
    console.queue_command(creator)
