# Surface Behavior Guide

## Surface Properties
- Shared surface property wrappers belong under `Property/` with surface-property names; prefer templated optics over material-specific source enums or duplicate cooked classes when only the input source varies.
- When texture sampling or value transforms are known at cook time, bake them into typed surface properties/adapters, including constant inputs, so optics avoid runtime source/transform dispatch.

## Optics Math
- Cooked surface optics should use natural math units internally, such as radians for angles; put artist-facing units and conversions at SDL, add-on, or documentation boundaries.
