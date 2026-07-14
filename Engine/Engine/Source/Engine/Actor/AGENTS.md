# Actor Guide

## Actor Cooking
- `preCook()` checks actor-owned completeness and prepares dependency-free data such as base transforms; cooked SDL dependency and actor-cache access belongs in `cook()`, which must be skipped for uncookable reports.
- For tracked SDL dependencies, actor, material, and motion-source `cook()` may assume `SdlDependencyResolver` already cooked resources into `CookingContext`; use `ctx.getCooked()` for access and fix resolver/resource registration if this is false, rather than adding local recursive cooking, defaults, or broad missing-dependency guards.

## Ownership Boundaries
- Factory helpers that return owning `std::unique_ptr<T>` across actor headers must make `T` complete at ownership destruction sites; include the owned base definition in the public boundary when callers receive ownership instead of relying on forward declarations.

## SDL Fields
- For SDL-backed actor/material fields, keep defaults in SDL field declarations such as `defaultTo(...)` rather than duplicating member initializers unless a non-SDL construction path requires one.
- For paired scalar/map fields, use the base field name for the scalar value and `<field>-map` for the mapped input; prefer non-optional scalar fields with SDL defaults, optional map fields, and one class-level precedence statement over repeated per-field wording. The mapped input takes precedence when both are specified.
- For `TSdlSpectrum`, interpret serialized input as tristimulus or SPD from its representation,
  never from actor-declared `EColorUsage`. `Raw` triples bypass color transforms with a tristimulus
  working color space; with a spectral working color space, they are reconstructed from the
  explicit tag or linear sRGB when untagged.
  Untagged EMR and ECF triples also default to linear sRGB.
- Do not apply `TSdlSpectrum`'s untagged-triple default to image resources; an unspecified image
  color space means Raw data.

## Blender PLY Models
- Actor/model cooking owns Blender material-slot metadata injection. Geometry may expose `CookedGeometry::faceIdToMetadataSlot`, but the actor combines it with material-slot metadata.
- Keep acceleration/intersection primitives material-agnostic; do not subclass KD-tree meshes just to carry Blender material arrays.
- Blender PLY face material IDs are Blender material slot IDs. Do not assume material slot count, unique referenced slot count, face count, and face-range count are equal.
- Blender PLY model metadata arrays are material-slot indexed. Sparse referenced slots still require actor material inputs covering those slots; do not compact/remap slots unless explicitly changing the actor contract.
- Empty material slots are invalid actor inputs unless an earlier export/cook step explicitly remaps or rejects referenced empty slots.
