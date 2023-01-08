from psdl.sdlconsole import SdlConsole
from psdl import sdl
from .. import naming

import bpy


def light_to_sdl_point_light(b_light: bpy.types.PointLight, console: SdlConsole):
    source_name = naming.get_mangled_light_name(b_light)

    creator = sdl.PointLightSourceCreator()
    creator.set_data_name(source_name)
    creator.set_color(sdl.Spectrum(b_light.ph_light_color_linear_srgb))
    creator.set_watts(sdl.Real(b_light.ph_light_watts))
    console.queue_command(creator)
