# Blender Add-on Guide

## Mesh Export
- Blender 4.5+ original mesh export writes one PLY geometry and one Blender PLY model actor with a material reference array. The legacy mesh path is the per-material actor export.
- Treat empty material slots by usage: unused empty slots should not force legacy export, while referenced empty slots need an explicit reject or fallback policy. Legacy export skips faces for referenced empty slots with a warning; it is not full support.
- `generated.pysdl` includes handwritten helpers from `SDLInterface/SDLGen/Resource/PythonGenerator/pysdl_base.py`; update the generator base and checked-in generated copy together when adding helper APIs.
