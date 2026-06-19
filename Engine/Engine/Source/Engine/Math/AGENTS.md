# Engine Math Guide

## Geometry and BVH
- For scalar AABB/ray hot paths, prefer explicit x/y/z slab code when profiling shows it matters; MSVC may keep stack materialization and dynamic indexing in generic loops even when the source looks equivalent.
- When nearest-hit and occlusion BVH traversal share the same node walk, keep one implementation with a compile-time occlusion flag; the occlusion specialization should return on first primitive hit and avoid nearest-hit probe/max-t state updates.

## Math Functions
- `TTabulatedMathFunction2D` is a nearest-cell lookup table over a 2D domain, stores sampled values as `float32`, and intentionally does not interpolate or normalize; callers are responsible for preserving kernel scale when needed.
- Sample filters tabulate non-box kernels through `SampleFilter::make(..., useTabulated)` with `use-tabulated-sample-filter` defaulting to true; keep the box filter direct because its exact math is already cheaper than table lookup.
