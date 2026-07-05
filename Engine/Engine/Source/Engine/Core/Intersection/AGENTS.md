# Intersection Guide

## Primitive Metadata
- `Primitive::toMetadataSlot(faceID)` is part of hit metadata resolution; multi-slot primitives must route face IDs before metadata injection is useful.
- Prefer metadata-injection slot mapping for per-face material metadata. Keep acceleration structures focused on intersection and face IDs, not material ownership.

## Data Structures
- `TIndexRangeMap` stores contiguous index ranges, not source elements or unique values. Allocate one entry per range and use tests where source count, unique value count, and range count differ.
- Keep index-buffer reads format-agnostic at generic geometry boundaries. Use exact-width typed fetches only where the encoded index width is already established.

## Occlusion Paths
- `isOccluding()` hot paths should avoid nearest-hit work such as `HitProbe` copies, metadata writes, and max-t updates; confirm primitive/BVH changes with target assembly and repeated real-render timings, not only source inspection.

## Packet Triangles
- For SIMD/packet triangle experiments, prefer an N-wide math geometry type adapted by existing primitive/BVH templates; gate runtime use by compiled SIMD capability plus geometry SDL accelerator settings, and keep build heuristics, mesh ordering, asset downloads, and perf scene setup as separate measured changes.
