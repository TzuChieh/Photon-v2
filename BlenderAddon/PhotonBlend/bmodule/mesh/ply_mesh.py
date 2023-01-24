from psdl.sdlresource import SdlResourceIdentifier

import bpy

from pathlib import Path
from dataclasses import dataclass


@dataclass
class PlyExportConfig:
    is_binary: bool = True
    include_positions: bool = True
    include_normals: bool = True
    include_texcoords: bool = True


def _write_ply_mesh_header(
    ply_file,
    vertices,
    indexed_faces,
    config: PlyExportConfig):

    file_format = b"binary_little_endian" if config.is_binary else b"ascii"
    blender_version = bpy.app.version_string.encode('utf-8')

    # Basic info

    ply_file.write(b"ply\n")
    ply_file.write(b"format %s 1.0\n" % file_format)
    ply_file.write(b"comment Exported by PhotonBlend from Blender %s\n" % blender_version)

    # Vertex declaration

    ply_file.write(b"element vertex %d\n" % len(vertices))

    if config.include_positions:
        ply_file.write(b"property float x\n")
        ply_file.write(b"property float y\n")
        ply_file.write(b"property float z\n")

    if config.include_normals:
        ply_file.write(b"property float nx\n")
        ply_file.write(b"property float ny\n")
        ply_file.write(b"property float nz\n")

    if config.include_texcoords:
        ply_file.write(b"property float u\n")
        ply_file.write(b"property float v\n")

    # Index declaration

    ply_file.write(b"element vertex %d\n" % len(indexed_faces))
    ply_file.write(b"property list uchar uint vertex_indices\n")
    ply_file.write(b"end_header\n")


def _write_ascii_ply_mesh_data(
    ply_file,
    vertices,
    indexed_faces,
    config: PlyExportConfig):

    # Vertices

    for position, normal, texcoord in vertices:
        if config.include_positions:
            ply_file.write(b"%.6f %.6f %.6f" % position)
        
        if config.include_normals:
            ply_file.write(b"%.6f %.6f %.6f" % normal)

        if config.include_texcoords:
            ply_file.write(b"%.6f %.6f" % texcoord)

    # Vertex indices

    for face_indices in indexed_faces:
        ply_file.write(b"%d" % len(face_indices))
        for face_index in face_indices:
            ply_file.write(b" %d" % face_index)
        ply_file.write(b"\n")



def _write_binary_ply_mesh_data(
    ply_file,
    vertices,
    indexed_faces,
    config: PlyExportConfig):

    # TODO
    pass


def export_ply_mesh(
    filepath: Path, 
    vertices,
    indexed_faces,
    config: PlyExportConfig):

    with open(filepath, 'wb') as ply_file:
        _write_ply_mesh_header(ply_file, vertices, indexed_faces, config)
        if config.is_binary:
            _write_binary_ply_mesh_data(ply_file, vertices, indexed_faces, config)
        else:
            _write_ascii_ply_mesh_data(ply_file, vertices, indexed_faces, config)




def export_ply_meshes(
    filepath: Path, 
    b_mesh: bpy.types.Mesh, 
    config: PlyExportConfig):




