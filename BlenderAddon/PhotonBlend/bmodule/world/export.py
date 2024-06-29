"""
@brief Convert Blender light data block to Photon's format.
"""
from psdl import sdl, SdlConsole

import bpy


def world_to_sdl_actor(b_world: bpy.types.World, console: SdlConsole):
    """
    Convert a world data block to SDL actor.
    """
    actor_name = "ph_" + b_world.name

    bg_type = b_world.photon.background_type
    if bg_type == 'NONE':
        creator = None
    elif bg_type == 'IMAGE' and b_world.photon.image_file_path != "":
        # TODO: not bundle/copy the same file if already present

        # Copy the IBL image file to scene folder and obtain an identifier for it
        image_path = bpy.path.abspath(b_world.photon.image_file_path)
        bundled_image_path = console.bundle_file(image_path, b_world.name + "_data")
        image_identifier = sdl.ResourceIdentifier()
        image_identifier.set_bundled_path(bundled_image_path)

        creator = sdl.ImageDomeActorCreator()
        creator.set_data_name(actor_name)
        creator.set_image_file(image_identifier)
    elif bg_type == 'PREETHAM':
        creator = sdl.PreethamDomeActorCreator()
        creator.set_data_name(actor_name)

        creator.set_turbidity(sdl.Real(b_world.photon.preetham_turbidity))
        creator.set_standard_time_24h(sdl.Real(b_world.photon.standard_time))
        creator.set_standard_meridian_degrees(sdl.Real(b_world.photon.standard_meridian))
        creator.set_site_latitude_degrees(sdl.Real(b_world.photon.latitude))
        creator.set_site_longitude_degrees(sdl.Real(b_world.photon.longitude))
        creator.set_julian_date(sdl.Integer(b_world.photon.julian_date))
    else:
        print(f"warning: world {b_world.name} has unsupported background type {bg_type}")
        return

    if creator is None:
        return

    creator.set_energy_scale(sdl.Real(b_world.photon.energy_scale))
    console.queue_command(creator)

    rotation = sdl.DomeActorRotate()
    rotation.set_target_name(actor_name)
    rotation.set_axis(sdl.Vector3((0, 1, 0)))
    rotation.set_degrees(sdl.Real(b_world.photon.up_rotation))
    console.queue_command(rotation)
