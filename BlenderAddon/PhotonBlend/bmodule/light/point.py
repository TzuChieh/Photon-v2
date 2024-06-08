from psdl import sdl, SdlConsole
from .. import naming

import bpy


def light_to_sdl_point_light(b_light: bpy.types.PointLight, console: SdlConsole):
    light_actor_name = naming.get_mangled_light_name(b_light)

    creator = sdl.PointLightActorCreator()
    creator.set_data_name(light_actor_name)
    creator.set_color(sdl.Spectrum(b_light.ph_light_color_linear_srgb))
    creator.set_watts(sdl.Real(b_light.ph_light_watts))
    console.queue_command(creator)
