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
    b_light = b_light_object.data
    if b_light.type == 'AREA':
        area.light_to_sdl_area_light(b_light, console)
    elif b_light.type == 'POINT':
        point.light_to_sdl_point_light(b_light, console)
    else:
        print("warning: light object <%s> has unsupported light type %s" % (b_light_object.name, b_light.type))
        return

    light_actor_name = naming.get_mangled_light_name(b_light)

    pos, rot, scale = blender.to_photon_pos_rot_scale(b_light_object.matrix_world)

    # Blender's rectangle area light is facing downwards (Blender's -z) by default, while Photon's rectangle 
    # is facing upwards (Blender's +z); these rotations account for such differences (for symmetric shape 
    # this works, otherwise the result of the rotations may make the shape upside down)
    if b_light.type == 'AREA':
        if b_light.shape == 'SQUARE' or b_light.shape == 'RECTANGLE':
            rot = rot @ mathutils.Quaternion((1.0, 0.0, 0.0), math.radians(180.0))

    translator = sdl.CallTranslate()
    translator.set_target_name(light_actor_name)
    translator.set_amount(sdl.Vector3(pos))
    console.queue_command(translator)

    rotator = sdl.CallRotate()
    rotator.set_target_name(light_actor_name)
    rotator.set_rotation(sdl.Quaternion((rot.x, rot.y, rot.z, rot.w)))
    console.queue_command(rotator)

    scaler = sdl.CallScale()
    scaler.set_target_name(light_actor_name)
    scaler.set_amount(sdl.Vector3(scale))
    console.queue_command(scaler)
