# Blender Add-on Guide

## Mesh Export
- Blender 4.5+ original mesh export writes one PLY geometry and one Blender PLY model actor with a material reference array; it falls back to legacy per-material actors on any empty material slot, emissive material, or masked material.
- Blender 4.1+ mesh normals should come from `Mesh.corner_normals`; only legacy Blender paths should prepare/read `calc_normals()`, `calc_normals_split()`, `MeshLoopTriangle.split_normals`, vertex normals, or triangle normals.
- The fast Blender PLY writer uses `psdl.direct().engine.GBlenderPlyPolygonMesh.write_ply()` from `bin.photon_renderer`; for Blender 4.5, build `SDLPyBind` with Python 3.11 and keep the add-on installation path pointed at the build root.
- `PhotonBlend/generated/pysdl.py` includes handwritten helpers from `SDLInterface/SDLGen/Resource/PythonGenerator/pysdl_base.py`; update the generator base and checked-in generated copy together when adding helper APIs.
- When C++ SDL declarations affect Blender export or UI, rebuild `SDLGenCLI` with the Blender-matching Python version, run `scripts/dev_update_blender_addon.py`, and rebuild `SDLPyBind` if Blender needs `bin.photon_renderer`.

## Material Nodes
- Material node exporters should use `PhMaterialNode` resource/default helpers; incomplete output-owning nodes warn with a reason and queue fallback for their output resource, while output nodes fallback to the owning material resource.
- Target the current generated SDL interface; do not add compatibility branches for obsolete SDL field names or old saved node socket layouts unless explicitly requested.
- For value/map material inputs where a UI value and map are alternatives, use one linkable socket with the needed default/range/subtype metadata; export linked resources through map setters and unlinked values through scalar setters, and do not create constant image resources solely to satisfy a mapped input.
- Put reusable semantic socket classes in `bmodule/material/node_base.py` with stable `bl_idname` and complete name/description/default/range/subtype metadata; keep sockets node-local only when their value contract is owner-specific.
- For `bpy.props.EnumProperty`, include stable numeric IDs on every item and never change existing IDs; see `BlenderAddon/README.md` before adding or reordering enum entries.
