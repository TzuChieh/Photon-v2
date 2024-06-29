"""
@brief Convert Blender mesh data block to Photon's format.
"""
from utility import blender, material
from psdl import sdl, SdlConsole
from bmodule import naming
from . import triangle_mesh

import bpy


def _export_light_actor(
    console: SdlConsole,
    light_actor_name,
    emission_image_name,
    geometry_name,
    material_name,
    position,
    rotation,
    scale):
    """
    Export a SDL light actor to `console`.
    """
    creator = sdl.ModelLightActorCreator()
    creator.set_data_name(light_actor_name)
    creator.set_emitted_energy(sdl.Image(emission_image_name))
    creator.set_geometry(sdl.Geometry(geometry_name))
    creator.set_material(sdl.Material(material_name))
    console.queue_command(creator)   

    translator = sdl.CallTranslate()
    translator.set_target_name(light_actor_name)
    translator.set_amount(sdl.Vector3(position))
    console.queue_command(translator)

    rotator = sdl.CallRotate()
    rotator.set_target_name(light_actor_name)
    rotator.set_rotation(sdl.Quaternion((rotation.x, rotation.y, rotation.z, rotation.w)))
    console.queue_command(rotator)

    scaler = sdl.CallScale()
    scaler.set_target_name(light_actor_name)
    scaler.set_amount(sdl.Vector3(scale))
    console.queue_command(scaler)


def _export_model_actor(
    console: SdlConsole,
    model_actor_name,
    geometry_name,
    material_name,
    position,
    rotation,
    scale):
    """
    Export a SDL model actor to `console`.
    """
    creator = sdl.ModelActorCreator()
    creator.set_data_name(model_actor_name)
    creator.set_geometry(sdl.Geometry(geometry_name))
    creator.set_material(sdl.Material(material_name))
    console.queue_command(creator)

    translator = sdl.CallTranslate()
    translator.set_target_name(model_actor_name)
    translator.set_amount(sdl.Vector3(position))
    console.queue_command(translator)

    rotator = sdl.CallRotate()
    rotator.set_target_name(model_actor_name)
    rotator.set_rotation(sdl.Quaternion((rotation.x, rotation.y, rotation.z, rotation.w)))
    console.queue_command(rotator)

    scaler = sdl.CallScale()
    scaler.set_target_name(model_actor_name)
    scaler.set_amount(sdl.Vector3(scale))
    console.queue_command(scaler)


def _export_original_mesh_object(b_mesh_object: bpy.types.Object, console: SdlConsole):
    """
    Export Blender original mesh object. This will group faces with the same material, then export each
    material-faces pair as a Photon actor.
    """
    b_mesh = b_mesh_object.data
    b_mesh.calc_loop_triangles()
    if not b_mesh.has_custom_normals:
        b_mesh.calc_normals()
    else:
        b_mesh.calc_normals_split()

    # TODO: might be faster if using len(obj.material_slots()) for array size and simply store each loop tris array
    # TODO: material can link to mesh or object, distinguish them
    material_idx_loop_triangles_map = {}
    for b_loop_triangle in b_mesh.loop_triangles:
        # This index refers to material slots (their stack order in the UI).
        material_idx = b_loop_triangle.material_index

        if material_idx not in material_idx_loop_triangles_map.keys():
            material_idx_loop_triangles_map[material_idx] = []

        material_idx_loop_triangles_map[material_idx].append(b_loop_triangle)

    for material_idx in material_idx_loop_triangles_map.keys():
        b_material = b_mesh_object.material_slots[material_idx].material
        loop_triangles = material_idx_loop_triangles_map[material_idx]

        # A material slot can be empty, this check is necessary.
        if b_material is None:
            print("warning: no material is in mesh object %s's material slot %d, not exporting" % (
                b_mesh_object.name, material_idx))
            continue

        # To ensure unique geometry name (which is required by Photon SDL), we use slot index as suffix. Note that
        # `bpy.types.Mesh.name` can be the same for different mesh data (even of it appears to have different names
        # in the outliner, tested in Blender 3.6). This is either a bug or due to support for geometry node, as geometry
        # can be procedurally generated and we cannot generate a good name for it. This is why we are using mesh object's
        # name as prefix. See Blender issue "Depsgraph returns wrong evaluated object name in bpy #100314" (https://projects.blender.org/blender/blender/issues/100314).
        # TODO: Whether this will affect object instancing need to be tested. We may export same mesh data multiple times now.
        geometry_name = naming.get_mangled_mesh_name(b_mesh, prefix=b_mesh_object.name, suffix=str(material_idx))
        material_name = naming.get_mangled_material_name(b_material)

        # Use the active one as the UV map for export.
        # TODO: support exporting multiple or zero UV maps/layers
        b_uv_layers = b_mesh.uv_layers
        b_active_uv_layer = b_uv_layers.active

        # TODO: support & check mesh without uv map
        # if len(b_mesh.uv_layers) == 0:
        #     print("warning: mesh (%s) has no uv maps, ignoring" % geometry_name)
        #     continue

        # TODO: support & check mesh without uv map
        # if b_active_uv_layer is None:
        #     print("warning: mesh (%s) has %d uv maps, but no one is active (no uv map will be exported)" % (
        #         geometry_name, len(b_uv_layers)))
        #     continue

        # TODO: support & check mesh with multiple uv maps
        if len(b_mesh.uv_layers) > 1:
            print("warning: mesh (%s) has %d uv maps, only the active one is exported" % (
                geometry_name, len(b_uv_layers)))

        triangle_mesh.loop_triangles_to_sdl_triangle_mesh(
            geometry_name,
            console,
            loop_triangles,
            b_mesh.vertices,
            # b_active_uv_layer.data)
            b_active_uv_layer.data if b_active_uv_layer is not None else None,# HACK
            b_mesh.has_custom_normals)

        # Creating actor (can be either model or light depending on emissivity)
        pos, rot, scale = blender.to_photon_pos_rot_scale(b_mesh_object.matrix_world)

        if material.is_emissive(b_material):
            light_actor_name = naming.get_mangled_object_name(b_mesh_object, prefix="Emissive", suffix=str(material_idx))
            emission_image_name = material.get_emission_image_res_name(b_material)
            _export_light_actor(console, light_actor_name, emission_image_name, geometry_name, material_name, pos, rot, scale)
        else:
            model_actor_name = naming.get_mangled_object_name(b_mesh_object, suffix=str(material_idx))
            _export_model_actor(console, model_actor_name, geometry_name, material_name, pos, rot, scale)


def _export_menger_sponge_mesh_object(b_mesh_object: bpy.types.Object, console: SdlConsole):
    """
    Export a Photon menger sponge.
    """
    # Use active material only
    b_material = b_mesh_object.active_material
    if b_material is None:
        print(f"warning: no material for menger sponge mesh object {b_mesh_object.name}, not exporting")
        return

    b_mesh = b_mesh_object.data

    # Note that `bpy.types.Mesh.name` can be the same for different mesh data (even of it appears to have different names
    # in the outliner, tested in Blender 3.6). This is either a bug or due to support for geometry node, as geometry
    # can be procedurally generated and we cannot generate a good name for it. This is why we are using mesh object's
    # name as prefix. See Blender issue "Depsgraph returns wrong evaluated object name in bpy #100314" (https://projects.blender.org/blender/blender/issues/100314).
    # TODO: Whether this will affect object instancing need to be tested. We may export same mesh data multiple times now.
    geometry_name = naming.get_mangled_mesh_name(b_mesh, prefix=b_mesh_object.name)
    material_name = naming.get_mangled_material_name(b_material)

    sponge = sdl.MengerSpongeGeometryCreator()
    sponge.set_data_name(geometry_name)
    sponge.set_iterations(sdl.Integer(b_mesh['ph_num_iterations']))
    console.queue_command(sponge)
    
    # Creating actor (can be either model or light depending on emissivity)
    pos, rot, scale = blender.to_photon_pos_rot_scale(b_mesh_object.matrix_world)

    if material.is_emissive(b_material):
        light_actor_name = naming.get_mangled_object_name(b_mesh_object, prefix="Emissive")
        emission_image_name = material.get_emission_image_res_name(b_material)
        _export_light_actor(console, light_actor_name, emission_image_name, geometry_name, material_name, pos, rot, scale)
    else:
        model_actor_name = naming.get_mangled_object_name(b_mesh_object)
        _export_model_actor(console, model_actor_name, geometry_name, material_name, pos, rot, scale)


def mesh_object_to_sdl_actor(b_mesh_object: bpy.types.Object, console: SdlConsole):
    """
    Export a Blender mesh object as SDL actor(s).
    """
    b_mesh = b_mesh_object.data
    if b_mesh is None:
        print("warning: mesh object (%s) has no mesh data, not exporting" % b_mesh_object.name)
        return

    if len(b_mesh.materials) == 0:
        print("warning: mesh object (%s) has no material, not exporting" % b_mesh_object.name)
        return

    match b_mesh.photon.export_type:
        case 'ORIGINAL':
            _export_original_mesh_object(b_mesh_object, console)
        case 'MENGER_SPONGE':
            _export_menger_sponge_mesh_object(b_mesh_object, console)
        case _:
            assert False, f"unsupported export type {b_mesh.photon.export_type}"
