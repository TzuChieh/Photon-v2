# Intersection Guide

## Primitive Metadata
- Route multi-slot face IDs through `Primitive::toMetadataSlot(faceID)` and metadata-injection
  mapping; keep acceleration structures responsible for intersections and face IDs, not material
  ownership.

## Data Structures
- `TIndexRangeMap` stores contiguous index ranges, not source elements or unique values. Allocate one entry per range and use tests where source count, unique value count, and range count differ.
- Keep index-buffer reads format-agnostic at generic geometry boundaries. Use exact-width typed fetches only where the encoded index width is already established.

## Primitive Attributes
- Expose primitive attributes through `Primitive::getAttribute(attribute, domain, faceID, out_values)`: queries are independent of hit events and barycentrics, return raw local opaque values, and transformed/decorator primitives forward them without coordinate conversion.
- Keep `TIndexedPolygonBuffer` domain-specific with `getFaceAttribute()` and `getFaceVertexAttributes()`; it translates face IDs through topology, while `IndexedAttributeBuffer` owns layout, data I/O, and value decoding without interpolation or weighting.
- Keep runtime attribute metadata and data in one cache-line-aligned allocation to avoid per-instance allocation overhead; construction-only counts, size/memory queries, raw writes, and mutation belong to `IndexedAttributeBufferWriter`. Require all entries to use the same layout mode.

## Occlusion Paths
- `isOccluding()` hot paths should avoid nearest-hit work such as `HitProbe` copies, metadata writes, and max-t updates; confirm primitive/BVH changes with target assembly and repeated real-render timings, not only source inspection.

## Packet Triangles
- For SIMD/packet triangle experiments, prefer an N-wide math geometry type adapted by existing primitive/BVH templates; gate runtime use by compiled SIMD capability plus geometry SDL accelerator settings, and keep build heuristics, mesh ordering, asset downloads, and perf scene setup as separate measured changes.
