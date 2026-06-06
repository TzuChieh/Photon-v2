# Actor Guide

## Blender PLY Models
- Actor/model cooking owns Blender material-slot metadata injection. Geometry may expose `CookedGeometry::faceIdToMetadataSlot`, but the actor combines it with material-slot metadata.
- Keep acceleration/intersection primitives material-agnostic; do not subclass KD-tree meshes just to carry Blender material arrays.
