from psdl import sdl, SdlConsole
from bmodule import naming

import bpy


def light_to_sdl_point_light(b_light: bpy.types.PointLight, console: SdlConsole):
    light_actor_name = naming.get_mangled_light_name(b_light)

    creator = sdl.PointLightActorCreator()
    creator.set_data_name(light_actor_name)
    creator.set_color(sdl.Spectrum(b_light.photon.color_linear_srgb))
    creator.set_watts(sdl.Real(b_light.photon.watts))

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
