# Engine Math Guide

## Geometry and BVH
- For scalar AABB/ray hot paths, prefer explicit x/y/z slab code when profiling shows it matters; MSVC may keep stack materialization and dynamic indexing in generic loops even when the source looks equivalent.
