# Blender Add-on Guide

## Mesh Export
- Blender 4.5+ original mesh export writes one PLY geometry and one Blender PLY model actor with a
  material reference array; preserve empty slots as `""` references and use legacy per-material
  actors for emissive materials. Interface masks belong to material output and must not force
  per-material splitting or disable instancing.
- Blender 4.1+ mesh normals should come from `Mesh.corner_normals`; only legacy Blender paths should prepare/read `calc_normals()`, `calc_normals_split()`, `MeshLoopTriangle.split_normals`, vertex normals, or triangle normals.
- The fast Blender PLY writer uses `psdl.direct().engine.GBlenderPlyPolygonMesh.write_ply()` from `bin.photon_renderer`; for Blender 4.5, build `SDLPyBind` with Python 3.11 and keep the add-on installation path pointed at the build root.
- `PhotonBlend/generated/pysdl.py` is ignored generated output containing helpers from
  `SDLInterface/SDLGen/Resource/PythonGenerator/pysdl_base.py`; edit the generator base and
  regenerate the output before Blender validation.
- When C++ SDL declarations affect Blender export or UI, rebuild `SDLGenCLI` with the Blender-matching Python version and run `scripts/dev_update_blender_addon.py`; if Blender runtime code consumes the changed definitions through `bin/photon_renderer`, also rebuild `SDLPyBind` for that Python version and restart Blender.

## Material Nodes
- Material node exporters should use `PhMaterialNode` resource/default helpers; incomplete output-owning nodes warn with a reason and queue fallback for their output resource, while output nodes fallback to the owning material resource.
- Target the current generated SDL interface; do not add compatibility branches for obsolete SDL field names or old saved node socket layouts unless explicitly requested.
- Direct `Spectrum` inputs may omit the default linear-sRGB tag; color-valued `ConstantImage`
  inputs must set `color-space`, since omission means Raw image data.
- For value/map material inputs where a UI value and map are alternatives, use one linkable socket with the needed default/range/subtype metadata; export linked resources through generated `set_<field>_map()` methods and unlinked values through scalar setters, reserve generic `packet.set_input()` for raw SDL struct packets, and do not create constant image resources solely to satisfy a mapped input.
- For Noise coordinates, export a coordinate resource only when explicitly linked; leave the SDL
  field unset when unlinked so the renderer's sample-UVW default remains active.
- Put reusable semantic socket classes in `bmodule/material/node_base.py` with stable `bl_idname` and complete name/description/default/range/subtype metadata; keep sockets node-local only when their value contract is owner-specific.
- For exported `bpy.props.EnumProperty` values, use the corresponding renderer SDL enum declaration
  as the authority for identifiers and user-facing meanings; include stable numeric IDs on every
  item and never change existing IDs. See `BlenderAddon/README.md` before adding or reordering entries.
