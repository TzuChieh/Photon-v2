# Geometry Guide

## Blender PLY
- Preserve Blender's split indexing: position indices reference vertices, while loop indices reference corner attributes. After triangulation, the loop-attribute count can differ from the index count.
