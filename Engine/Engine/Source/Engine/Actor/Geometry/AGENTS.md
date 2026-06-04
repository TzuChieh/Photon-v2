# Geometry Guide

## Blender PLY
- Treat Blender PLY as a writer-private fixed format. Keep loading contract-driven and fast; avoid compatibility parsing or redundant validation in the hot path.
- Preserve Blender's split authoring data on export, but cook to the renderer's unified loop-indexed mesh representation.
- For material slots, distinguish face count, unique slot count, and contiguous face-range count; cooked metadata mapping is face-range based.
