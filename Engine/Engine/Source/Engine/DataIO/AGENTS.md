# Data IO Guide

## PLY
- Treat `PlyFile` as the generic ASCII/binary reader; Blender export uses a separate fixed binary
  writer under `Actor/Geometry`, so do not infer or call generic `PlyFile` writing support.
