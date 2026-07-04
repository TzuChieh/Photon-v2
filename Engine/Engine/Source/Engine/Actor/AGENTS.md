# Actor Guide

## Actor Cooking
- `preCook()` checks actor-owned completeness and prepares dependency-free data such as base transforms; cooked SDL dependency and actor-cache access belongs in `cook()`, which must be skipped for uncookable reports.
- For tracked SDL dependencies, actor, material, and motion-source `cook()` may assume `SdlDependencyResolver` already cooked resources into `CookingContext`; use `ctx.getCooked()` for access and fix resolver/resource registration if this is false, rather than adding local recursive cooking, defaults, or broad missing-dependency guards.

## Blender PLY Models
- Actor/model cooking owns Blender material-slot metadata injection. Geometry may expose `CookedGeometry::faceIdToMetadataSlot`, but the actor combines it with material-slot metadata.
- Keep acceleration/intersection primitives material-agnostic; do not subclass KD-tree meshes just to carry Blender material arrays.
- Blender PLY face material IDs are Blender material slot IDs. Do not assume material slot count, unique referenced slot count, face count, and face-range count are equal.
- Blender PLY model metadata arrays are material-slot indexed. Sparse referenced slots still require actor material inputs covering those slots; do not compact/remap slots unless explicitly changing the actor contract.
- Empty material slots are invalid actor inputs unless an earlier export/cook step explicitly remaps or rejects referenced empty slots.
