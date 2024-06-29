from psdl import sdl, SdlConsole

import bpy


def light_to_sdl_point_light_actor(
    b_light: bpy.types.PointLight,
    console: SdlConsole,
    actor_name,
    phantomize=False):
    """
    Convert a light data block to SDL point light actor.
    """
    assert b_light.type == 'POINT'

    light_radius = b_light.shadow_soft_size
    if light_radius == 0:
        creator = sdl.PointLightActorCreator()
    else:
        creator = sdl.SphereLightActorCreator()
        creator.set_radius(sdl.Real(light_radius))

    creator.set_data_name(actor_name)
    creator.set_color(sdl.Spectrum(b_light.photon.color_linear_srgb))
    creator.set_watts(sdl.Real(b_light.photon.watts))

    if phantomize:
        creator.phantomize()

    # Advanced feature flags: set if different from Photon's default to reduce file size

    if not b_light.photon.directly_visible:
        creator.set_directly_visible(sdl.Bool(False))

    if not b_light.photon.point_light_bsdf_sample:
        creator.set_bsdf_sample(sdl.Bool(False))

    if not b_light.photon.direct_sample:
        creator.set_direct_sample(sdl.Bool(False))

    if not b_light.photon.emission_sample:
        creator.set_emission_sample(sdl.Bool(False))

    console.queue_command(creator)
