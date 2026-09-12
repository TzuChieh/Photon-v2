# Geometry Guide

## Geometry Bounds
- For shader-generated coordinates derived from geometry bounds, use tight, undeformed
  geometry-local bounds that remain invariant under instancing rather than robustness-padded
  traversal AABBs.

## Blender PLY
- Treat Blender PLY as a writer-private fixed format. Keep loading contract-driven and fast; avoid compatibility parsing or redundant validation in the hot path.
- Preserve Blender's split authoring data on export, but cook to the renderer's unified loop-indexed mesh representation and derive face-domain counts from the polygon index buffer.
- Preserve the fixed Blender PLY element/property and payload order; append optional `tri_customs` face attributes last and omit the element and payload entirely when no custom slot has values.
- For material slots, distinguish face count, unique slot count, and contiguous face-range count; cooked metadata mapping is face-range based.
