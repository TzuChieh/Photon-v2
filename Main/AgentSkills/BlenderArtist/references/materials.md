# Blender-to-Photon Material Conversion

Read this only for converting or repairing Blender/Cycles materials in Photon. Use it as a compact conversion pass plus a record of behavior that is easy to miss from node labels or shallow graph inspection.

## Establish the effective source

- Start at the active Material Output and reverse-trace only behavior-bearing paths. Include surface, volume, emission, opacity, normals, and their controls; ignore nodes unreachable from that output.
- Treat a linked socket's expression as effective; its displayed default has no effect. Record every effective source input as linked, constant, unsupported, or intentionally irrelevant before replacing the graph.
- Resolve use through exact material datablocks, object slots, and actual polygons. Do not treat `Material.users` as an object or face count. Inspect evaluated geometry when modifiers can change topology or material indices, and inspect every linked instance when transforms affect coordinates or normals.
- Establish texture encoding, color space, coordinate domain, and visible-side intent from authoritative asset data and effective graph behavior. Treat imported or legacy nodes as evidence rather than authority when they conflict.
- Treat every verified nonconstant authored value, including subtle variation, as mapped; do not collapse it to a constant without explicit authorization.
- Classify every effective behavior as exact, qualitative, or unsupported. A representable prefix does not make a complete path exact when a later stage or consumer remains unsupported.
- Author the replacement in `PH_MATERIAL_NODE_TREE`; preserve assignments, masks, emission, normal behavior, and object bindings unless explicitly in scope.

## Graph and resource semantics

- Before approximating, baking, mixing, or declaring behavior unsupported, enumerate the installed classes in `BlenderAddon/PhotonBlend/bmodule/material/nodes.py` and inspect each plausible candidate's live RNA, controlling modes, socket identifiers, `to_sdl()`, and corresponding engine implementation. Prefer the most direct node or mapped input that preserves the complete effective behavior; treat this reference as pitfalls, not a support inventory. Duplicate labels can name different sockets, and Photon exports only the socket selected by the controlling mode.
- Recreate saved custom nodes that lack newly added sockets, then restore links by meaning rather than socket index.
- Build Binary Mixed Surface as `A * Factor + B * (1 - Factor)`. Put the factor-1 branch in A and factor-0 branch in B. Select Float for scalar controls such as Split, Luminance, and Noise Value; Color produces independent per-channel material weights.
- Derive expected node modes and active sockets from source semantics before inspecting the existing Photon graph; an internally consistent stale or wrong mode is not evidence of correctness.
- Reuse a Picture only when file, color space, Raw Data, sample mode, and wrap mode match for every consumer. Create sidecar textures only for behavior Photon cannot express, and remove only resources proven unreferenced or byte-identical after rewiring.

## Image, scalar, and color lowering

- Derive ORM packing, opacity location, and normal handedness from asset metadata; never infer them from filenames, grayscale appearance, or OpenPBR.
- Use a Raw Data Picture directly for an authored scalar stored in channel 0; Photon scalar sampling reads channel 0. Use Split Image for a verified packed channel. Use Luminance after color-space conversion for genuinely color-authored data.
- Preserve declared color management before color-to-scalar conversion. Cycles uses scene-linear RGB-to-Y; Photon Luminance is exact only with matching working primaries. Do not mark a color-managed grayscale image Raw Data merely because sampled channels match.
- Lower a Cycles linear Map Range to its affine function and preserve Clamp. When From bounds are equal, preserve Cycles' output of 0. Treat other interpolation modes as qualitative or unsupported unless represented exactly.
- Lower full-factor Invert after the required scalar conversion as `1 - x`. For distinct stops, lower a two-stop linear Color Ramp as `y0 + (y1 - y0) * clamp((x - p0) / (p1 - p0), 0, 1)` and decompose linear multi-stop ramps into clamped segment deltas. Treat unrepresentable interpolation as qualitative or unsupported.
- Map HSV as `Hue = source Hue - 0.5` normalized turns, with Saturation and Value unchanged and Factor mapped to Amount. Account for Photon's linear-sRGB evaluation and `[0, 1]` Amount clamp.
- Map RGB Curves to Color Remap at the same graph stage. Copy Combined before R/G/B, preserve Factor and fan-out, and give each Photon node its own curve tree. Photon exports a 257-sample LUT with extrapolation slopes in linear sRGB; compare sampled functions before calling the result exact.
- Preserve the order and fan-out of every reachable conversion and blend. Do not bypass a representable stage or call a color path complete while a downstream blend remains unsupported.

## Surface-model differences

- Choose the surface family by physical role before building an approximation: use Ideal Substance dielectric for a perfectly smooth interface, Thin Dielectric Surface for an authored thin sheet, Abraded Translucent for a rough interface, and Layered Surface only when every authored layer maps to Surface Layer's roughness, complex IOR, thickness, asymmetry, absorption, and scattering. Surface Layer has no diffuse albedo. Use Pure Absorber only for actual complete absorption, never to hide geometry or substitute for transmission.
- Map Diffuse Surface Oren-Nayar Roughness as sigma.
- Map Abraded Opaque F0, roughness, and Reflection Scale; treat Reflection Scale as an artistic, non-physical control.
- For a Principled-style metalness workflow whose source semantics match, fan the fully processed Base Color and Roughness into both branches: use Base Color as metal F0 and dielectric diffuse albedo, apply Roughness to both microfacet lobes, then mix the complete metal branch in A against the complete dielectric branch in B using Metalness. Classify every other active Principled lobe separately.
- Use a white microfacet proxy for glossy-over-diffuse only as a qualitative fallback. For constant inputs, use normal-incidence factor `((IOR - 1) / (IOR + 1))^2 * (2 * Specular IOR Level)`; this omits angle-dependent Fresnel and energy compensation.
- Select `SQUARED` only when the source value is perceptual roughness; Photon implements it as roughness squared. Do not apply it to an already converted microfacet alpha.
- Set Ideal Substance's type explicitly; the node defaults to `metallic-reflector`. Do not replace a volume-only source with an opaque surface.
- Treat Ideal Medium as absorption-only. An equal-IOR, unit-transmission surface is only an intersectable boundary passthrough: it consumes a surface bounce and occludes binary shadow queries.
- Map Abraded Translucent Fresnel, IOR, roughness, and Reflection/Transmission Scale. Treat the scales as non-physical per-hit lobe-energy controls, not tint, thickness, absorption, scattering, or Beer-Lambert attenuation; spatial transmission color remains unsupported.
- Do not fold ambient occlusion into base color unless the source graph explicitly does so; Photon has no general OpenPBR AO material input.

## Coordinates and procedural inputs

- Preserve Blender defaults deliberately: an unconnected Image Texture uses active UV and an unconnected Noise uses Generated coordinates, while Photon Picture and Noise without an explicit coordinate resource use hit UVW.
- Use Image Transform only for one affine function of hit UVW. Photon applies scale, XYZ rotation, then translation; it cannot select a coordinate domain, reconstruct a missing component, invert an unwrap, or provide per-object transforms to one shared material.
- Use Attribute `UVW From Geometry Bound` for hit position normalized by each hit geometry's tight, undeformed geometry-local AABB; actor transforms and instancing do not change that bound, and a zero-extent axis yields `0.5`. Do not bake one object's bound into a shared material. Use `Geometry Hit Position` for unnormalized local position. Arbitrary named objects and attributes remain unsupported.
- Do not compensate for a coordinate-domain mismatch by changing Noise Frequency unless the complete relation is derived; otherwise label the result qualitative.
- Account for the absence of mipmapping and ray differentials. Matching coordinates alone does not match minified or grazing-angle appearance.
- Map Cycles Perlin fBM Noise Dimensions and Normalize directly; map Scale to Frequency, Detail to `Num Layers = Detail + 1`, Roughness to Amplitude Ratio, Lacunarity to Frequency Ratio, and Distortion to Warp. Map Fac to Value. Photon Color broadcasts the scalar and does not reproduce Cycles colored Noise.

## Normals, masks, and emission

- Wrap the complete Photon surface with Normal Mapped Surface. Select `opengl`, `directx`, or `directx-rg` from source metadata.
- Treat Normal Map Strength as qualitative for `0 < Strength < 1` unless sampled outputs match: Photon scales tangent components, while Cycles also moves the normal component toward neutral.
- Preserve scalar Bump as unsupported; Normal Mapped Surface does not evaluate height, so never route height into Normal Map or Strength.
- Feed Surface Mask an alpha-extracted scalar image. A Picture consumed as scalar reads channel 0, not alpha.
- Lower emission color and linked strength separately, then multiply them. Classify emission per material slot and polygon. Any masked or emissive slot triggers legacy per-material triangle grouping; a group that is both emissive and masked is exported as an unmasked `ModelLight`, so its mask is ignored.
- Separate actor-transform parity, cooked winding parity, local winding, `Ng`, and `Ns`. Photon preserves transformed `Ng`; `should-flip-ng` reverses only `Ng`, with cooked winding parity XORed into the effective value, and PhotonBlend does not derive it from `matrix_world`.
- Audit one-sided emission against transformed `Ns`, not `Ng`. Determine the visible emissive face geometrically and recalculate normals only for components proven uniformly inverted.

## Verify the conversion

- Verify the active Photon output, referenced resources, effective constants and maps, assignments, masks, emission, coordinate domains, and normal behavior. Apply roughness conversion exactly once.
- Re-derive every Picture-to-scalar choice and every Binary Mix mode, active factor socket, A/B order, and extracted channel from source semantics. Validate Color Remap stage and fan-out rather than node presence alone.
- Confirm every new node is reverse-reachable from a used Photon output path. Remove only obsolete, unreachable nodes and external resources after all consumers are rewired.
- Report remaining qualitative and unsupported behavior explicitly; do not describe a partial mapping as complete.

These rules are version-sensitive. Verify discrepancies in `BlenderAddon/PhotonBlend/bmodule/material/`, `BlenderAddon/PhotonBlend/bmodule/mesh/export.py`, and the corresponding `Engine/Engine/Source/Engine/` implementations.
