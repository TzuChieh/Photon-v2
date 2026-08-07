"""
@brief Convert Blender mesh data block to Photon's format.
"""
from utility import blender, material
from psdl import sdl, SdlConsole
from bmodule import naming
from . import triangle_mesh
import psdl

import bpy
import numpy as np


def _supports_ply_export():
    return bpy.app.version >= (4, 5, 0)


def _supports_mesh_instancing():
    return _supports_ply_export()


def _supports_corner_normals():
    return bpy.app.version >= (4, 1, 0)


def _queue_light_actor(
    console: SdlConsole,
    b_mesh_obj: bpy.types.Object,
    light_actor_name,
    emission_image_name,
    geometry_name,
    material_name,
    position,
    rotation,
    scale):
    """
    Queue an SDL light actor to `console`.
    """
    creator = sdl.ModelLightActorCreator()
    creator.set_data_name(light_actor_name)
    creator.set_display_name(sdl.String(b_mesh_obj.name))
    creator.set_emitted_energy(sdl.Image(emission_image_name))
    creator.set_geometry(sdl.Geometry(geometry_name))
    creator.set_material(sdl.Material(material_name))
    console.queue_command(creator)   

    _queue_transform_commands(console, light_actor_name, position, rotation, scale)


def _queue_model_actor(
    console: SdlConsole,
    b_mesh_obj: bpy.types.Object,
    model_actor_name,
    geometry_name,
    material_name,
    position,
    rotation,
    scale):
    """
    Queue an SDL model actor to `console`.
    """
    creator = sdl.ModelActorCreator()
    creator.set_data_name(model_actor_name)
    creator.set_display_name(sdl.String(b_mesh_obj.name))
    creator.set_geometry(sdl.Geometry(geometry_name))
    creator.set_material(sdl.Material(material_name))
    console.queue_command(creator)

    _queue_transform_commands(
        console,
        model_actor_name,
        position,
        rotation,
        scale)


def _queue_transform_commands(console: SdlConsole, actor_name, position, rotation, scale):
    translator = sdl.CallTranslate()
    translator.set_target_name(actor_name)
    translator.set_amount(sdl.Vector3(position))
    console.queue_command(translator)

    rotator = sdl.CallRotate()
    rotator.set_target_name(actor_name)
    rotator.set_rotation(sdl.Quaternion((rotation.x, rotation.y, rotation.z, rotation.w)))
    console.queue_command(rotator)

    scaler = sdl.CallScale()
    scaler.set_target_name(actor_name)
    scaler.set_amount(sdl.Vector3(scale))
    console.queue_command(scaler)


def _write_blender_ply_file(
    ply_path,
    raw_vert_positions,
    raw_vert_loop_normals,
    raw_vert_loop_uvs,
    vert_position_indices,
    vert_loop_indices,
    tri_mat_ids):
    psdl.direct().engine.GBlenderPlyPolygonMesh.write_ply(
        path=ply_path,
        raw_vert_positions=raw_vert_positions,
        raw_vert_loop_normals=raw_vert_loop_normals,
        raw_vert_loop_uvs=raw_vert_loop_uvs,
        vert_position_indices=vert_position_indices,
        vert_loop_indices=vert_loop_indices,
        tri_mat_ids=tri_mat_ids)


def _queue_blender_ply_geometry(
    console: SdlConsole,
    b_mesh: bpy.types.Mesh,
    geometry_name,
    bundled_ply_path):
    geometry_creator = sdl.BlenderPlyGeometryCreator()
    geometry_creator.set_data_name(geometry_name)
    geometry_creator.set_display_name(sdl.String(b_mesh.name))

    ply_file = sdl.ResourceIdentifier()
    ply_file.set_bundled_path(bundled_ply_path)
    geometry_creator.set_ply_file(ply_file)

    console.queue_command(geometry_creator)


def _queue_blender_ply_model_actor(
    console: SdlConsole,
    b_mesh_obj: bpy.types.Object,
    actor_name,
    geometry_name,
    b_materials,
    *,
    is_phantom=False,
    is_instantiable=False):
    material_refs = sdl.ReferenceArray("material")
    for b_material in b_materials:
        material_name = naming.get_mangled_material_name(b_material) if b_material is not None else ""
        material_refs.add(sdl.Material(material_name))

    actor_creator = sdl.BlenderPlyModelActorCreator()
    actor_creator.set_data_name(actor_name)
    actor_creator.set_display_name(sdl.String(b_mesh_obj.name))
    actor_creator.set_geometry(sdl.Geometry(geometry_name))
    actor_creator.set_materials(material_refs)
    if is_instantiable:
        actor_creator.set_is_instantiable_hint(sdl.Bool(True))
    if is_phantom:
        actor_creator.phantomize()

    console.queue_command(actor_creator)


def _get_mesh_obj_materials(b_mesh_obj: bpy.types.Object):
    b_materials = [b_material_slot.material for b_material_slot in b_mesh_obj.material_slots]

    # Blender faces default to material index 0 even when the mesh has no material slots, so we need [None]
    return b_materials or [None]


def _get_mesh_obj_ply_export_materials(b_mesh_obj: bpy.types.Object):
    """
    Return materials for PLY mesh export, or `None` when the mesh requires another export path.
    """
    b_mesh = b_mesh_obj.data
    if b_mesh is None or b_mesh.photon.export_type != 'ORIGINAL':
        return None

    b_materials = _get_mesh_obj_materials(b_mesh_obj)
    for b_material in b_materials:
        if b_material is None:
            continue

        if material.is_emissive(b_material):
            return None

    return b_materials


def _export_original_mesh_obj_as_ply(
    b_mesh_obj: bpy.types.Object,
    console: SdlConsole,
    *,
    b_materials,
    b_world_matrix,
    name_suffix,
    is_instance_source=False):
    """
    Export the mesh as one PLY geometry and one model actor.
    Vertex attributes are transferred to C++ in bulk for faster I/O.
    @return The exported actor's SDL resource name.
    """
    b_mesh = b_mesh_obj.data

    # TODO: maybe we can avoid this by exporting ngon
    b_mesh.calc_loop_triangles()

    # Get raw data

    num_raw_loops = len(b_mesh.loops)
    num_raw_vert_positions = len(b_mesh.vertices)

    # Bulk get all vertex positions
    raw_vert_positions = np.empty(num_raw_vert_positions * 3, dtype=np.float32)
    b_mesh.vertices.foreach_get('co', raw_vert_positions)

    # Bulk get all vertex normals
    raw_vert_loop_normals = np.empty(num_raw_loops * 3, dtype=np.float32)
    b_mesh.corner_normals.foreach_get('vector', raw_vert_loop_normals)

    # Bulk get all UVs (use the active one as the UV map for export)
    # TODO: support exporting multiple or zero UV maps/layers
    raw_vert_loop_uvs = np.zeros(num_raw_loops * 2, dtype=np.float32)
    b_uv_layers = b_mesh.uv_layers
    b_active_uv_layer = b_uv_layers.active
    if b_active_uv_layer and b_active_uv_layer.data:
        b_active_uv_layer.data.foreach_get('uv', raw_vert_loop_uvs)

    # Get indices into raw data

    num_tris = len(b_mesh.loop_triangles)

    # Bulk get all triangle vertex position indices
    vert_position_indices = np.empty(num_tris * 3, dtype=np.uint32)
    b_mesh.loop_triangles.foreach_get('vertices', vert_position_indices)

    # Bulk get per-vertex loop indices for attributes like UV, color...
    vert_loop_indices = np.empty(num_tris * 3, dtype=np.uint32)
    b_mesh.loop_triangles.foreach_get('loops', vert_loop_indices)

    # Bulk get all material IDs (1 for each face)
    tri_mat_ids = np.empty(num_tris, dtype=np.uint32)
    b_mesh.loop_triangles.foreach_get('material_index', tri_mat_ids)

    geometry_name = naming.get_mangled_mesh_name(b_mesh_obj, name_suffix)
    ply_path = console.get_working_dir() / "Mesh_data" / f"{geometry_name}.ply"
    ply_path.parent.mkdir(parents=True, exist_ok=True)
    bundled_ply_path = console.get_bundled_path(ply_path)
    _write_blender_ply_file(
        ply_path,
        raw_vert_positions,
        raw_vert_loop_normals,
        raw_vert_loop_uvs,
        vert_position_indices,
        vert_loop_indices,
        tri_mat_ids)

    _queue_blender_ply_geometry(
        console,
        b_mesh,
        geometry_name,
        bundled_ply_path)

    model_actor_name = naming.get_mangled_actor_name(b_mesh_obj, name_suffix)
    _queue_blender_ply_model_actor(
        console,
        b_mesh_obj,
        model_actor_name,
        geometry_name,
        b_materials,
        is_phantom=is_instance_source,
        is_instantiable=is_instance_source)

    if not is_instance_source:
        pos, rot, scale = blender.to_photon_pos_rot_scale(b_world_matrix)
        _queue_transform_commands(console, model_actor_name, pos, rot, scale)
    return model_actor_name


def _export_original_mesh_obj_per_material(
    b_mesh_obj: bpy.types.Object,
    console: SdlConsole,
    *,
    b_world_matrix,
    name_suffix):
    """
    Export a Blender original mesh object. This groups faces with the same material, then exports
    each material-faces pair as a Photon actor. It manually loops over each triangle and saves all
    data in raw SDL, which can be slow and produce large files for larger scenes. Nevertheless,
    this is a good reference as it handles everything explicitly.
    """
    b_mesh = b_mesh_obj.data
    b_materials = _get_mesh_obj_materials(b_mesh_obj)
    b_mesh.calc_loop_triangles()

    if _supports_corner_normals():
        b_mesh_corner_normals = b_mesh.corner_normals
    else:
        b_mesh_corner_normals = None
        # Older Blender needs explicit normal preparation.
        if not b_mesh.has_custom_normals:
            b_mesh.calc_normals()
        else:
            b_mesh.calc_normals_split()

    # TODO: might be faster if using len(obj.material_slots()) for array size and simply store each loop tris array
    # TODO: material can link to mesh or object, distinguish them
    material_idx_to_loop_triangles = {}
    for b_loop_triangle in b_mesh.loop_triangles:
        # This index refers to material slots (their stack order in the UI).
        material_idx_to_loop_triangles.setdefault(b_loop_triangle.material_index, []).append(b_loop_triangle)

    pos, rot, scale = blender.to_photon_pos_rot_scale(b_world_matrix)
    for material_idx, b_loop_triangles in material_idx_to_loop_triangles.items():
        b_material = b_materials[material_idx]

        # Empty material slots do not produce actors
        if b_material is None:
            print(
                f"warning: material index {material_idx} on mesh object {b_mesh_obj.name} "
                "has no assigned material, not exporting its faces")
            continue

        # Evaluated meshes can share `Mesh.name`. Object identity distinguishes different mesh
        # objects, while `segment_suffix` distinguishes depsgraph instances and mesh segments.
        # See also: Blender issue "Depsgraph returns wrong evaluated object name in bpy #100314"
        # (https://projects.blender.org/blender/blender/issues/100314).
        segment_suffix = naming.join_name_parts(
            name_suffix,
            naming.join_name_parts("segment", material_idx) if len(material_idx_to_loop_triangles) > 1 else None)
        geometry_name = naming.get_mangled_mesh_name(b_mesh_obj, segment_suffix)
        material_name = naming.get_mangled_material_name(b_material)

        # Use the active one as the UV map for export.
        # TODO: support exporting multiple or zero UV maps/layers
        b_uv_layers = b_mesh.uv_layers
        b_active_uv_layer = b_uv_layers.active

        # TODO: support & check mesh with multiple uv maps
        if len(b_mesh.uv_layers) > 1:
            print("warning: mesh (%s) has %d uv maps, only the active one is exported" % (
                geometry_name, len(b_uv_layers)))

        triangle_mesh.loop_triangles_to_sdl_triangle_mesh(
            geometry_name,
            b_mesh,
            console,
            b_loop_triangles,
            b_mesh.vertices,
            b_active_uv_layer.data if b_active_uv_layer is not None else None,
            b_mesh.has_custom_normals,
            b_mesh_corner_normals)

        # Create either a model or light actor, depending on emissivity.
        if material.is_emissive(b_material):
            light_actor_name = naming.get_mangled_actor_name(b_mesh_obj, "emissive", segment_suffix)
            emission_image_name = material.get_emission_image_res_name(b_material)
            _queue_light_actor(
                console,
                b_mesh_obj,
                light_actor_name,
                emission_image_name,
                geometry_name,
                material_name,
                pos,
                rot,
                scale)
        else:
            model_actor_name = naming.get_mangled_actor_name(b_mesh_obj, segment_suffix)
            _queue_model_actor(
                console,
                b_mesh_obj,
                model_actor_name,
                geometry_name,
                material_name,
                pos,
                rot,
                scale)


def _export_original_mesh_obj(
    b_mesh_obj: bpy.types.Object,
    console: SdlConsole,
    *,
    b_world_matrix,
    name_suffix):
    """
    Export the mesh with its original appearance in Blender.
    """
    b_ply_materials = _get_mesh_obj_ply_export_materials(b_mesh_obj)
    if _supports_ply_export() and b_ply_materials is not None:
        _export_original_mesh_obj_as_ply(
            b_mesh_obj,
            console,
            b_materials=b_ply_materials,
            b_world_matrix=b_world_matrix,
            name_suffix=name_suffix)
    else:
        # Export each material-faces pair as a Photon actor
        _export_original_mesh_obj_per_material(
            b_mesh_obj,
            console,
            b_world_matrix=b_world_matrix,
            name_suffix=name_suffix)


def _export_menger_sponge_mesh_obj(
    b_mesh_obj: bpy.types.Object,
    console: SdlConsole,
    *,
    b_world_matrix,
    name_suffix):
    """
    Export a Photon Menger sponge.
    """
    # Use only the active material
    b_material = b_mesh_obj.active_material
    if b_material is None:
        print(f"warning: no material for menger sponge mesh object {b_mesh_obj.name}, not exporting")
        return

    b_mesh = b_mesh_obj.data

    # Evaluated meshes can share `Mesh.name`, so use mesh object identity for geometry names.
    # See Blender issue "Depsgraph returns wrong evaluated object name in bpy #100314" (https://projects.blender.org/blender/blender/issues/100314).
    # Add the name suffix for repeated direct exports.
    geometry_name = naming.get_mangled_mesh_name(b_mesh_obj, name_suffix)
    material_name = naming.get_mangled_material_name(b_material)

    sponge = sdl.MengerSpongeGeometryCreator()
    sponge.set_data_name(geometry_name)
    sponge.set_display_name(sdl.String(b_mesh.name))
    sponge.set_iterations(sdl.Integer(b_mesh['ph_num_iterations']))
    console.queue_command(sponge)
    
    # Create either a model or light actor, depending on emissivity
    pos, rot, scale = blender.to_photon_pos_rot_scale(b_world_matrix)

    if material.is_emissive(b_material):
        light_actor_name = naming.get_mangled_actor_name(b_mesh_obj, "emissive", name_suffix)
        emission_image_name = material.get_emission_image_res_name(b_material)
        _queue_light_actor(
            console,
            b_mesh_obj,
            light_actor_name,
            emission_image_name,
            geometry_name,
            material_name,
            pos,
            rot,
            scale)
    else:
        model_actor_name = naming.get_mangled_actor_name(b_mesh_obj, name_suffix)
        _queue_model_actor(
            console,
            b_mesh_obj,
            model_actor_name,
            geometry_name,
            material_name,
            pos,
            rot,
            scale)


def get_mesh_obj_source_key(b_mesh_obj: bpy.types.Object):
    """
    Return an export key identifying equivalent mesh source data.
    """
    b_mesh = b_mesh_obj.data
    if b_mesh is None:
        return None

    match b_mesh.photon.export_type:
        case 'ORIGINAL':
            return (
                b_mesh,
                tuple(b_mat_slot.material for b_mat_slot in b_mesh_obj.material_slots))
        case 'MENGER_SPONGE':
            # Menger sponge exports only its active material
            return (
                b_mesh,
                b_mesh_obj.active_material)
        case _:
            return None


def can_mesh_obj_be_instance_source(b_mesh_obj: bpy.types.Object):
    """
    Return whether this mesh can be exported as one instance source actor.
    """
    b_mesh = b_mesh_obj.data
    if not _supports_mesh_instancing() or b_mesh is None:
        return False

    match b_mesh.photon.export_type:
        case 'ORIGINAL':
            return _get_mesh_obj_ply_export_materials(b_mesh_obj) is not None
        case _:
            return False


def mesh_obj_to_sdl_actor(
    b_mesh_obj: bpy.types.Object,
    console: SdlConsole,
    *,
    b_world_matrix,
    name_suffix=None):
    """
    Export one or more visible actors at a Blender world transform.
    """
    b_mesh = b_mesh_obj.data
    if b_mesh is None:
        print(f"warning: mesh object ({b_mesh_obj.name}) has no mesh data, not exporting")
        return

    match b_mesh.photon.export_type:
        case 'ORIGINAL':
            _export_original_mesh_obj(
                b_mesh_obj,
                console,
                b_world_matrix=b_world_matrix,
                name_suffix=name_suffix)
        case 'MENGER_SPONGE':
            _export_menger_sponge_mesh_obj(
                b_mesh_obj,
                console,
                b_world_matrix=b_world_matrix,
                name_suffix=name_suffix)
        case _:
            print(
                f"warning: mesh object {b_mesh_obj.name} has unsupported export type "
                f"{b_mesh.photon.export_type!r}, not exporting")


def mesh_obj_to_sdl_instance_source(
    b_mesh_obj: bpy.types.Object,
    console: SdlConsole,
    *,
    name_suffix=None):
    """
    Export a phantom mesh actor for instances to reference.
    It is an error if `can_mesh_obj_be_instance_source()` returns false on the mesh object.
    @return The exported source actor's SDL resource name.
    """
    if not can_mesh_obj_be_instance_source(b_mesh_obj):
        raise ValueError(
            f"mesh object {b_mesh_obj.name} cannot be used as an instance source")

    b_ply_materials = _get_mesh_obj_ply_export_materials(b_mesh_obj)
    assert b_ply_materials is not None
    return _export_original_mesh_obj_as_ply(
        b_mesh_obj,
        console,
        b_materials=b_ply_materials,
        b_world_matrix=None,
        name_suffix=name_suffix,
        is_instance_source=True)


def queue_transformed_instance_actor(
    console: SdlConsole,
    *,
    source_actor_name,
    actor_name,
    display_name,
    b_world_matrices):
    """
    Queue one transformed instance actor containing all supplied transforms.
    """
    transforms = sdl.StructArray()
    for transform_index, b_world_matrix in enumerate(b_world_matrices):
        pos, rot, scale = blender.to_photon_pos_rot_scale(b_world_matrix)
        transform_packet_name = naming.join_name_parts(actor_name, "transform", transform_index)

        transform_packet = sdl.CachedPacketCommand()
        transform_packet.set_data_name(transform_packet_name)
        transform_packet.set_input("pos", sdl.Vector3(pos))
        transform_packet.set_input("rot", sdl.Quaternion((rot.x, rot.y, rot.z, rot.w)))
        transform_packet.set_input("scale", sdl.Vector3(scale))
        console.queue_command(transform_packet)
        transforms.add(sdl.CachedPacket(transform_packet_name))

    actor_creator = sdl.TransformedInstanceActorCreator()
    actor_creator.set_data_name(actor_name)
    actor_creator.set_display_name(sdl.String(display_name))
    actor_creator.set_source(sdl.Actor(source_actor_name))
    actor_creator.set_transforms(transforms)
    console.queue_command(actor_creator)
