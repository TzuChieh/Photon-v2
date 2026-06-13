# Blender Add-on Guide

## Mesh Export
- Blender 4.5+ original mesh export writes one PLY geometry and one Blender PLY model actor with a material reference array; it falls back to legacy per-material actors on any empty material slot, emissive material, or masked material.
- `generated.pysdl` includes handwritten helpers from `SDLInterface/SDLGen/Resource/PythonGenerator/pysdl_base.py`; update the generator base and checked-in generated copy together when adding helper APIs.

## Material Nodes
- Material node exporters should use `PhMaterialNode` resource/default helpers; incomplete output-owning nodes warn with a reason and queue fallback for their output resource, while output nodes fallback to the owning material resource.
