from ...psdl.sdlconsole import SdlConsole
from ...psdl.pysdl import (
        SDLVector3,
        SDLQuaternion,
        SDLLightSource)
from ...psdl.pysdl import (
        LightActorCreator,
        LightActorTranslate,
        LightActorRotate,
        LightActorScale)
from .. import naming
from ... import utility
from . import area, point

import bpy
import math
import mathutils


def light_object_to_sdl_actor(b_light_object: bpy.types.Object, console: SdlConsole):
    b_light = b_light_object.data
    if b_light.type == 'AREA':
        area.light_to_sdl_area_light(b_light, console)
    elif b_light.type == 'POINT':
        point.light_to_sdl_point_light(b_light, console)
    else:
        print("warning: light object <%s> has unsupported light type %s" % (b_light_object.name, b_light.type))
        return

    source_name = naming.get_mangled_light_name(b_light)
    actor_name = naming.get_mangled_object_name(b_light_object)

    pos, rot, scale = b_light_object.matrix_world.decompose()

    # Blender's rectangle area light is in its xy-plane (facing -z axis) by default, while Photon's rectangle is in
    # Blender's yz-plane (facing +x axis); these rotations accounts for such difference
    if b_light.type == 'AREA':
        if b_light.shape == 'SQUARE' or b_light.shape == 'RECTANGLE':
            rot = rot @ mathutils.Quaternion((1.0, 0.0, 0.0), math.radians(90.0))
            rot = rot @ mathutils.Quaternion((0.0, 0.0, 1.0), math.radians(-90.0))

    pos = utility.to_photon_vec3(pos)
    rot = utility.to_photon_quat(rot)
    scale = utility.to_photon_vec3(scale)

    creator = LightActorCreator()
    creator.set_data_name(actor_name)
    creator.set_light_source(SDLLightSource(source_name))
    console.queue_command(creator)

    translator = LightActorTranslate()
    translator.set_target_name(actor_name)
    translator.set_factor(SDLVector3(pos))
    console.queue_command(translator)

    rotator = LightActorRotate()
    rotator.set_target_name(actor_name)
    rotator.set_factor(SDLQuaternion((rot.x, rot.y, rot.z, rot.w)))
    console.queue_command(rotator)

    scaler = LightActorScale()
    scaler.set_target_name(actor_name)
    scaler.set_factor(SDLVector3(scale))
    console.queue_command(scaler)
