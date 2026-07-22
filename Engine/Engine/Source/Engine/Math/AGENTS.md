# Engine Math Guide

## Color
- Set `PH_WORKING_COLOR_SPACE` to a bare `EColorSpace` entry: `Linear_sRGB`, `ACEScg`, or
  `Spectral`; use `PH_WORKING_COLOR_SPACE_ENUM` for the qualified C++ enum expression.
- Derive `math::Spectrum` from `math::working_color_space`; do not add parallel render-mode constants.
- Treat `EColorUsage` as physical/conversion semantics independent of representation; select
  tristimulus versus spectral storage from the color space or spectrum type, never from `Raw`,
  `EMR`, or `ECF`.

## Geometry and BVH
- For scalar AABB/ray hot paths, prefer explicit x/y/z slab code when profiling shows it matters; MSVC may keep stack materialization and dynamic indexing in generic loops even when the source looks equivalent.
- When nearest-hit and occlusion BVH traversal share the same node walk, keep one implementation with a compile-time occlusion flag; the occlusion specialization should return on first primitive hit and avoid nearest-hit probe/max-t state updates.
- Validate BVH performance with same-build `PhotonCLI` controls using the same scene, thread count,
  and configuration plus repeated minimum render times; microbenchmarks or width alone are not
  proof.
- In wide BVH traversal, use hit-mask/set-bit iteration for occlusion, but retain per-child min-T
  values in nearest ordered traversal so hits can tighten max-T pruning.
- Keep wide BVH order lookup tables as `static constexpr` values built by `consteval` helpers;
  re-check generated assembly after table or layout changes because selected rows may still be
  copied.
- Treat `TWideBvhNode` metadata/accessor changes as cache-sensitive, full-render decisions: helper hotspots such as `isLeaf()` are not enough evidence if the node layout change increases memory pressure or loses elsewhere.

## Math Functions
- In CRTP math-base implementations, keep explicit casts to the base's `Self` type when accessing
  base-owned protected storage through a `Derived` object; indirect derived types can re-expose the
  member with different access.
- `TTabulatedMathFunction2D` is a nearest-cell lookup table over a 2D domain, stores sampled values as `float32`, and intentionally does not interpolate or normalize; callers are responsible for preserving kernel scale when needed.
- Sample filters tabulate non-box kernels through `SampleFilter::make(..., useTabulated)` with `use-tabulated-sample-filter` defaulting to true; keep the box filter direct because its exact math is already cheaper than table lookup.
- Keep vector normalization routed through shared length helpers; when profiling shows fixed-extent loops compile poorly, specialize the helper with direct expressions instead of duplicating math at callsites.
- Keep straightforward finite checks unless assembly and renderer timing justify a less obvious rewrite.

## Procedural Noise
- Keep raw procedural-noise primitives in the math layer; texture/material layers own output
  scaling, fBM aggregation, distortion, and use-case-specific coordinate policy.
