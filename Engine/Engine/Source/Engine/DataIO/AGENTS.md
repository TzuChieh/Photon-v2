# Data IO Guide

## PLY
- `PlyFile` is the generic PLY reader today: it loads ASCII and binary input into native binary buffers, while generic file writing is not implemented yet.
- Keep PLY enum-to-keyword and keyword-to-enum helpers paired; future PLY writing needs header keywords even if current loading only calls the parse side.
- Blender PLY export is a separate writer-private fixed binary path under `Actor/Geometry`; do not infer generic `PlyFile` writer support from it.
