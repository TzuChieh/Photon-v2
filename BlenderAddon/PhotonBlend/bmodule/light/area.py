from psdl import sdl, SdlConsole

import bpy


def light_to_sdl_area_light_actor(
    b_light: bpy.types.AreaLight,
    console: SdlConsole,
    actor_name,
    phantomize=False):
    """
    Convert a light data block to SDL area light actor.
    """
    assert b_light.type == 'AREA'

    if b_light.shape in {'SQUARE', 'RECTANGLE'}:
        # In Blender's Light, under Area category, only Square and Rectangle shape are available.
        # (which are both a rectangle in Photon)
        rec_width = b_light.size
        rec_height = b_light.size_y if b_light.shape == 'RECTANGLE' else b_light.size

        creator = sdl.RectangleLightActorCreator()
        creator.set_data_name(actor_name)
        creator.set_width(sdl.Real(rec_width))
        creator.set_height(sdl.Real(rec_height))
        creator.set_color(sdl.Spectrum(b_light.photon.color_linear_srgb))
        creator.set_watts(sdl.Real(b_light.photon.watts))

        # Advanced feature flags: set if different from Photon's default to reduce file size

        if not b_light.photon.directly_visible:
            creator.set_directly_visible(sdl.Bool(False))

        if not b_light.photon.bsdf_sample:
            creator.set_bsdf_sample(sdl.Bool(False))

        if not b_light.photon.direct_sample:
            creator.set_direct_sample(sdl.Bool(False))

        if not b_light.photon.emission_sample:
            creator.set_emission_sample(sdl.Bool(False))

    else:
        print(f"warning: light {b_light.name} has shape {b_light.shape} which is not supported")
        return

    if phantomize:
        creator.phantomize()

    console.queue_command(creator)
