# Surface Behavior Guide

## Surface Properties
- Shared surface property wrappers belong under `Property/` with surface-property names; prefer templated optics over material-specific source enums or duplicate cooked classes when only the input source varies.
- Select typed constant/textured properties at cook time and bake known transforms into them so optics avoid runtime source/transform dispatch; when independently typed properties are supported, keep constants texture-free rather than synthesizing constant images/textures, and mark stored property/policy members `PH_NO_UNIQUE_ADDRESS` so empty types add no size.
- When one optics instantiation requires a homogeneous source mode across a property group, any map selects textured mode and promotes remaining constants; choose constant mode only when no map is present.
- Treat omitted artistic scales as exact unit scale; use explicit typed properties only for non-unit constants or mapped controls.

## Optics Math
- Cooked surface optics should use natural math units internally, such as radians for angles; put artist-facing units and conversions at SDL, add-on, or documentation boundaries.
