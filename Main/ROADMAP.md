# Project Roadmap

This document tracks upcoming architectural improvements and technical goals for the Photon-v2 engine.

## Math Library Improvements

The core math library (`Engine/Engine/Source/Engine/Math`) is a critical performance path for the renderer. The following area is currently prioritized for modernization and optimization:

### 1. Batch SIMD AABB Intersection (High Priority)
Decouple SIMD AABB logic from BVH-specific contexts into general-purpose utilities.

#### Rationale
The current SIMD AABB intersection logic is tightly coupled within `TBvhSimdComputingContext`, making it difficult to reuse in other performance-critical areas like frustum culling or other acceleration structures. By refactoring this logic into standalone, highly optimized classes, we can:
- **Increase Flexibility:** Separate the broadcasted ray state from the batch AABB data.
- **Improve Reusability:** Allow any engine component to perform batched AABB tests without dependency on BVH nodes.
- **Enhance Maintainability:** Provide a single, clean source of truth for SIMD-accelerated geometric tests.

#### Architectural Change
- **`TSoAAABB3D<N>`:** A pure data structure storing `N` AABBs in Structure of Arrays (SoA) format for efficient SIMD loading.
- **`TSimdRay3D`:** A pre-processed ray state that broadcasts origin and direction into SIMD registers, allowing a single ray to be efficiently tested against multiple boxes.

#### Phases
- **Research:** Identify optimal SIMD intrinsics for SSE/AVX and verify alignment requirements for SoA layouts.
- **Implementation:** Create `TSoAAABB3D.h` and `TSimdRay3D.h` in `Engine/Math/Geometry/`.
- **Integration:** Refactor `TBvhSimdComputingContext` to use these new types, removing redundant internal math logic.
- **Verification:** Unit tests in `EngineTest` to ensure bit-wise parity with scalar `TAABB3D` robust intersection algorithms.

---

*Last Updated: March 3, 2026*
