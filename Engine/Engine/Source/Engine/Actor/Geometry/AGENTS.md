# Geometry Guide

## Blender PLY
- Treat Blender PLY as a writer-private fixed format. Keep loading contract-driven and fast; avoid compatibility parsing or redundant validation in the hot path.
- Preserve Blender's split authoring data on export, but cook to the renderer's unified loop-indexed mesh representation.
