from ... import utility
from ...psdl.sdlconsole import SdlConsole
from ...psdl.pysdl import TriangleMeshGeometryCreator
from ...psdl.pysdl import SDLVector3Array

from mathutils import Vector, Quaternion


def buffers_to_sdl_triangle_mesh(
        resource_name,
        console: SdlConsole,
        **buffers):
    
    creator = TriangleMeshGeometryCreator()
    creator.set_data_name(resource_name)

    positions = SDLVector3Array()
    for b_position in buffers["positions"]:
        positions.add(utility.to_photon_vec3(b_position))
    creator.set_positions(positions)

    tex_coords = SDLVector3Array()
    for b_tex_coord in buffers["tex_coords"]:
        tex_coords.add(b_tex_coord)
    creator.set_texture_coordinates(tex_coords)

    normals = SDLVector3Array()
    for b_normal in buffers["normals"]:
        normals.add(utility.to_photon_vec3(b_normal))
    creator.set_normals(normals)

    console.queue_command(creator)


def loop_triangles_to_sdl_triangle_mesh(
        resource_name,
        console: SdlConsole,
        loop_triangles,
        b_mesh_vertices,
        b_mesh_uv_loops):

    positions = []
    tex_coords = []
    normals = []
    for b_loop_triangle in loop_triangles:
        """
        Vertices and loops can be different since loops are primarily for shading--UVs/colors across an edge might be 
        different (or hard edges), while vertices generally do not.
        """
        for vertex_index in b_loop_triangle.vertices:
            b_mesh_vertex = b_mesh_vertices[vertex_index]
            positions.append(b_mesh_vertex.co)
            normals.append(b_mesh_vertex.normal if b_loop_triangle.use_smooth else b_loop_triangle.normal)

        for loop_index in b_loop_triangle.loops:
            b_uv = b_mesh_uv_loops[loop_index].uv
            tex_coords.append(Vector((b_uv[0], b_uv[1], 0.0)))

    buffers_to_sdl_triangle_mesh(
        resource_name,
        console,
        positions=positions,
        tex_coords=tex_coords,
        normals=normals)

