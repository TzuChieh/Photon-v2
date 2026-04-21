import pytest
import sys
import os

# Note: The 'engine' fixture is provided by conftest.py

def test_import_engine(engine):
    assert engine is not None

def test_blender_ply_exists(engine):
    assert hasattr(engine, "GBlenderPlyPolygonMesh")
    
    mesh_class = engine.GBlenderPlyPolygonMesh
    assert hasattr(mesh_class, "write_ply")

def test_blender_ply_instantiation(engine):
    # Should be able to create with default values
    mesh = engine.GBlenderPlyPolygonMesh()
    assert mesh is not None

def test_blender_ply_write_ply_signature(engine):
    mesh_class = engine.GBlenderPlyPolygonMesh
    # write_ply is a static method
    assert hasattr(mesh_class, "write_ply")
    
    # We can't easily check the signature of a nanobind-defined function 
    # as it might not follow standard inspect rules, but we can check it's callable.
    assert callable(mesh_class.write_ply)

def test_blender_ply_write_ply_call(engine, tmp_path):
    try:
        import numpy as np
    except ImportError:
        pytest.skip("numpy not found, skipping call test")

    mesh_class = engine.GBlenderPlyPolygonMesh
    ply_path = tmp_path / "test.ply"
    
    # Mock data: 1 triangle
    positions = np.array([0, 0, 0, 1, 0, 0, 0, 1, 0], dtype=np.float32)
    normals = np.array([0, 0, 1, 0, 0, 1, 0, 0, 1], dtype=np.float32)
    uvs = np.array([0, 0, 1, 0, 0, 1], dtype=np.float32)
    pos_indices = np.array([0, 1, 2], dtype=np.uint32)
    loop_indices = np.array([0, 1, 2], dtype=np.uint32)
    mat_ids = np.array([0], dtype=np.uint32)
    
    # Write the file (passing Path directly to test binder conversion)
    mesh_class.write_ply(
        path=ply_path,
        raw_vert_positions=positions,
        raw_vert_loop_normals=normals,
        raw_vert_loop_uvs=uvs,
        vert_position_indices=pos_indices,
        vert_loop_indices=loop_indices,
        tri_mat_ids=mat_ids
    )
    
    assert ply_path.exists()

    # VERIFICATION: Read the file back and check content
    with open(ply_path, 'rb') as f:
        content = f.read()
        
    # Find the end of the header (header ends with 'end_header\n')
    header_end_marker = b'end_header\n'
    header_end_idx = content.find(header_end_marker) + len(header_end_marker)
    header = content[:header_end_idx].decode('ascii')
    binary_data = content[header_end_idx:]

    # Check header contents
    assert "element raw_vert_positions 3" in header
    assert "element raw_vert_loop_normals 3" in header
    assert "element mat_ids 1" in header
    assert "end_header" in header

    # Verify binary data blocks (in the exact order written in C++)
    offset = 0
    
    # 1. rawVertPositions (3 * 3 float32)
    read_positions = np.frombuffer(binary_data, dtype=np.float32, count=9, offset=offset)
    assert np.allclose(read_positions, positions)
    offset += 9 * 4

    # 2. rawVertLoopNormals (3 * 3 float32)
    read_normals = np.frombuffer(binary_data, dtype=np.float32, count=9, offset=offset)
    assert np.allclose(read_normals, normals)
    offset += 9 * 4

    # 3. rawVertLoopUVs (3 * 2 float32)
    read_uvs = np.frombuffer(binary_data, dtype=np.float32, count=6, offset=offset)
    assert np.allclose(read_uvs, uvs)
    offset += 6 * 4

    # 4. vertPositionIndices (3 uint32)
    read_pos_indices = np.frombuffer(binary_data, dtype=np.uint32, count=3, offset=offset)
    assert np.array_equal(read_pos_indices, pos_indices)
    offset += 3 * 4

    # 5. vertLoopIndices (3 uint32)
    read_loop_indices = np.frombuffer(binary_data, dtype=np.uint32, count=3, offset=offset)
    assert np.array_equal(read_loop_indices, loop_indices)
    offset += 3 * 4

    # 6. triMatIds (1 uint32)
    read_mat_ids = np.frombuffer(binary_data, dtype=np.uint32, count=1, offset=offset)
    assert np.array_equal(read_mat_ids, mat_ids)

def test_blender_ply_inconsistent_data(engine, tmp_path):
    try:
        import numpy as np
    except ImportError:
        pytest.skip("numpy not found, skipping call test")

    mesh_class = engine.GBlenderPlyPolygonMesh
    
    # Inconsistent data: 2 positions but 3 indices
    positions = np.array([0, 0, 0, 1, 0, 0], dtype=np.float32)
    pos_indices = np.array([0, 1, 2], dtype=np.uint32)
    
    with pytest.raises(RuntimeError, match="Inconsistent Blender PLY polygon data sizes"):
        mesh_class.write_ply(
            path=tmp_path / "error.ply",
            raw_vert_positions=positions,
            raw_vert_loop_normals=np.array([], dtype=np.float32),
            raw_vert_loop_uvs=np.array([], dtype=np.float32),
            vert_position_indices=pos_indices,
            vert_loop_indices=pos_indices,
            tri_mat_ids=np.array([0], dtype=np.uint32)
        )
