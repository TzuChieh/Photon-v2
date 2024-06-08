import utility
from psdl import sdl, SdlConsole

from mathutils import Vector, Quaternion


def buffers_to_sdl_triangle_mesh(
        resource_name,
        console: SdlConsole,
        **buffers):
    
    creator = sdl.TriangleMeshGeometryCreator()
    creator.set_data_name(resource_name)

    positions = sdl.Vector3Array()
    for b_position in buffers["positions"]:
        positions.add(b_position)
    creator.set_positions(positions)

    tex_coords = sdl.Vector3Array()
    for b_tex_coord in buffers["tex_coords"]:
        tex_coords.add(b_tex_coord)
    creator.set_texture_coordinates(tex_coords)

    normals = sdl.Vector3Array()
    for b_normal in buffers["normals"]:
        normals.add(b_normal)
    creator.set_normals(normals)

    console.queue_command(creator)


def loop_triangles_to_sdl_triangle_mesh(
        resource_name,
        console: SdlConsole,
        loop_triangles,
        b_mesh_vertices,
        b_mesh_uv_loops,
        has_custom_normals=False):

    positions = []
    tex_coords = []
    normals = []
    for b_loop_triangle in loop_triangles:
        """
        `vertices` and `loops` can be different since loops are primarily for shading--UVs/colors across an 
        edge might be different (or hard edges), while vertices generally do not.
        """
        for vertex_index in b_loop_triangle.vertices:
            b_mesh_vertex = b_mesh_vertices[vertex_index]
            positions.append(b_mesh_vertex.co)
            if not has_custom_normals:
                b_normal = b_mesh_vertex.normal if b_loop_triangle.use_smooth else b_loop_triangle.normal
                normals.append(Vector((b_normal[0], b_normal[1], b_normal[2])))

        if has_custom_normals:
            for i in range(3):
                b_normal = b_loop_triangle.split_normals[i]
                normals.append(Vector((b_normal[0], b_normal[1], b_normal[2])))

        for loop_index in b_loop_triangle.loops:
            # b_uv = b_mesh_uv_loops[loop_index].uv
            b_uv = b_mesh_uv_loops[loop_index].uv if b_mesh_uv_loops is not None else (0.0, 0.0)# HACK
            tex_coords.append(Vector((b_uv[0], b_uv[1], 0.0)))

    buffers_to_sdl_triangle_mesh(
        resource_name,
        console,
        positions=positions,
        tex_coords=tex_coords,
        normals=normals)

