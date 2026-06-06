# Intersection Guide

## Primitive Metadata
- `Primitive::toMetadataSlot(faceID)` is part of hit metadata resolution; multi-slot primitives must route face IDs before metadata injection is useful.
- Prefer metadata-injection slot mapping for per-face material metadata. Keep acceleration structures focused on intersection and face IDs, not material ownership.

## Data Structures
- `TIndexRangeMap` stores contiguous index ranges, not source elements or unique values. Allocate one entry per range and use tests where source count, unique value count, and range count differ.
