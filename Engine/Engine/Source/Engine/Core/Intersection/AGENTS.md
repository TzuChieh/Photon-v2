# Intersection Guide

## Primitive Metadata
- Route multi-slot face IDs through `Primitive::toMetadataSlot(faceID)` and metadata-injection
  mapping; keep acceleration structures responsible for intersections and face IDs, not material
  ownership.

## Data Structures
- `TIndexRangeMap` stores contiguous index ranges, not source elements or unique values. Allocate one entry per range and use tests where source count, unique value count, and range count differ.
- Keep index-buffer reads format-agnostic at generic geometry boundaries. Use exact-width typed fetches only where the encoded index width is already established.

## Occlusion Paths
- `isOccluding()` hot paths should avoid nearest-hit work such as `HitProbe` copies, metadata writes, and max-t updates; confirm primitive/BVH changes with target assembly and repeated real-render timings, not only source inspection.
