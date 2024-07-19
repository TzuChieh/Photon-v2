"""
@brief Convert Blender light data block to Photon's format.
"""
from psdl import sdl, SdlConsole
from bmodule import naming
from bmodule.light import area, point
from utility import blender

import bpy
import mathutils

import math


def light_object_to_sdl_actor(b_light_object: bpy.types.Object, console: SdlConsole):
    """
    Convert a light object to SDL actor.
    """
    b_light = b_light_object.data
    attenuation_type = b_light.photon.attenuation_type
    use_attenuation = attenuation_type in {'IES'}
    light_actor_name = naming.get_mangled_light_name(b_light)
    src_light_actor_name = light_actor_name + "_src" if use_attenuation else light_actor_name
    if b_light.type == 'AREA':
        area.light_to_sdl_area_light_actor(b_light, console, src_light_actor_name, phantomize=use_attenuation)
    elif b_light.type == 'POINT':
        point.light_to_sdl_point_light_actor(b_light, console, src_light_actor_name, phantomize=use_attenuation)
    else:
        print(f"warning: light object {b_light_object.name} has unsupported light type {b_light.type}")
        return

    pos, rot, scale = blender.to_photon_pos_rot_scale(b_light_object.matrix_world)

    # Blender's rectangle area light is facing downwards (Blender's -z) by default, while Photon's rectangle 
    # is facing upwards (Blender's +z); these rotations account for such differences (for symmetric shape 
    # this works, otherwise the result of the rotations may make the shape upside down)
    if b_light.type == 'AREA':
        if b_light.shape in {'SQUARE', 'RECTANGLE'}:
            rot = rot @ mathutils.Quaternion((1.0, 0.0, 0.0), math.radians(180.0))

    # TODO: not exporting if no such transform

    translator = sdl.CallTranslate()
    translator.set_target_name(src_light_actor_name)
    translator.set_amount(sdl.Vector3(pos))
    console.queue_command(translator)

    rotator = sdl.CallRotate()
    rotator.set_target_name(src_light_actor_name)
    rotator.set_rotation(sdl.Quaternion((rot.x, rot.y, rot.z, rot.w)))
    console.queue_command(rotator)

    scaler = sdl.CallScale()
    scaler.set_target_name(src_light_actor_name)
    scaler.set_amount(sdl.Vector3(scale))
    console.queue_command(scaler)

    if not use_attenuation:
        return
    
    assert light_actor_name != src_light_actor_name

    if attenuation_type == 'IES':
        creator = sdl.IesAttenuatedLightActorCreator()
        creator.set_data_name(light_actor_name)

        ies_path = bpy.path.abspath(b_light.photon.ies_file_path)
        ies_identifier = sdl.ResourceIdentifier()
        ies_identifier.set_bundled_path(console.bundle_file(ies_path, b_light.name + "_data"))
        creator.set_ies_file(ies_identifier)

        creator.set_source(sdl.Actor(src_light_actor_name))
    else:
        print(f"warning: light object {b_light_object.name} has unsupported attenuation type {attenuation_type}")
        return

    console.queue_command(creator)
