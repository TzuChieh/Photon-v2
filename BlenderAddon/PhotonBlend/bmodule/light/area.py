from psdl import sdl, SdlConsole
from .. import naming

import bpy


def light_to_sdl_area_light(b_light: bpy.types.AreaLight, console: SdlConsole):
    light_actor_name = naming.get_mangled_light_name(b_light)

    if b_light.shape == 'SQUARE' or b_light.shape == 'RECTANGLE':
        # In Blender's Light, under Area category, only Square and Rectangle shape are available.
        # (which are both a rectangle in Photon)
        rec_width = b_light.size
        rec_height = b_light.size_y if b_light.shape == 'RECTANGLE' else b_light.size

        creator = sdl.RectangleLightActorCreator()
        creator.set_data_name(light_actor_name)
        creator.set_width(sdl.Real(rec_width))
        creator.set_height(sdl.Real(rec_height))
        creator.set_color(sdl.Spectrum(b_light.ph_light_color_linear_srgb))
        creator.set_watts(sdl.Real(b_light.ph_light_watts))
    else:
        print("warning: light <%s> has shape %s which is not supported" % b_light.name)
        creator = None

    if creator is not None:
        console.queue_command(creator)
