# Actor Guide

## Actor Cooking
- `preCook()` checks actor-owned completeness and prepares dependency-free data such as base transforms; cooked SDL dependency and actor-cache access belongs in `cook()`, which must be skipped for uncookable reports.
- For tracked SDL dependencies, actor, material, and motion-source `cook()` may assume `SdlDependencyResolver` already cooked resources into `CookingContext`; use `ctx.getCooked()` for access and fix resolver/resource registration if this is false, rather than adding local recursive cooking, defaults, or broad missing-dependency guards.

## Ownership Boundaries
- Factory helpers that return owning `std::unique_ptr<T>` across actor headers must make `T` complete at ownership destruction sites; include the owned base definition in the public boundary when callers receive ownership instead of relying on forward declarations.

## SDL Fields
- For paired scalar/map fields, use the base field name for the scalar value and `<field>-map` for the mapped input; prefer non-optional scalar fields with SDL defaults, optional map fields, and one class-level precedence statement over repeated per-field wording. The mapped input takes precedence when both are specified.
- For `TSdlSpectrum`, interpret serialized input as tristimulus or SPD from its representation,
  never from actor-declared `EColorUsage`. `Raw` triples bypass color transforms with a tristimulus
  working color space; with a spectral working color space, they are reconstructed from the
  explicit tag or linear sRGB when untagged.
  Untagged EMR and ECF triples also default to linear sRGB.
- Do not apply `TSdlSpectrum`'s untagged-triple default to image resources; an unspecified image
  color space means Raw data.

## Image Resources
- For image decorators that only change sampling coordinates, wrap the canonical numeric/color
  textures with a sample-location operator; inherit `Image`'s real/vector conversions unless
  distinct semantics or measured performance justify specialization.

## Blender PLY Models
- Actor/model cooking combines `CookedGeometry::faceIdToMetadataSlot` with slot-indexed materials
  and injects metadata; keep geometry and acceleration/intersection primitives material-agnostic.
- Treat face material IDs, actor material inputs, and metadata arrays as Blender-slot-indexed,
  including sparse and empty slots; do not compact/remap them, and use fully transmissive surface
  optics for empty slots.
