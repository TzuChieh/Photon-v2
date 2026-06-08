# Actor Guide

## Blender PLY Models
- Actor/model cooking owns Blender material-slot metadata injection. Geometry may expose `CookedGeometry::faceIdToMetadataSlot`, but the actor combines it with material-slot metadata.
- Keep acceleration/intersection primitives material-agnostic; do not subclass KD-tree meshes just to carry Blender material arrays.
- Blender PLY face material IDs are Blender material slot IDs. Do not assume material slot count, unique referenced slot count, face count, and face-range count are equal.
- Empty material slots are invalid actor inputs unless an earlier export/cook step explicitly remaps or rejects referenced empty slots.
