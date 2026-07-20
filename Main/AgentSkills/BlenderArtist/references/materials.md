# Photon Material Authoring

Read this reference completely only for material creation, conversion, repair, or audit.

## Contents

- [Establish source intent](#establish-source-intent)
- [Build Photon graphs](#build-photon-graphs)
- [Use Binary Mixed Surface correctly](#use-binary-mixed-surface-correctly)
- [Convert image values](#convert-image-values)
- [Lower scalar conversion chains](#lower-scalar-conversion-chains)
- [Map supported surface behavior](#map-supported-surface-behavior)
- [Map procedural inputs](#map-procedural-inputs)
- [Preserve secondary behavior](#preserve-secondary-behavior)
- [Audit and report](#audit-and-report)
- [Evidence anchors](#evidence-anchors)

## Establish source intent

1. Read the asset documentation and referenced material specification. Inspect raw asset material properties, texture associations, and files.
2. Audit one material at a time from the active Material Output. Reverse-trace every behavior-bearing input and record its state as linked, unlinked with an effective default, or unsupported. A displayed default has no effect while that socket is linked.
3. Record every object and material slot using the material. Use object and material names, geometry role, UV domain, and visible-side intent as supporting context when the graph alone is ambiguous; topology alone does not establish meaning.
4. Treat imported Cycles, Principled, or legacy FBX nodes as evidence, not authority, when they conflict with authoritative source data or observable source behavior.
5. Derive channel packing, alpha meaning, normal handedness, and color spaces from authoritative metadata. Do not assume a universal ORM layout, opacity-in-alpha convention, or DirectX/OpenGL normal format; OpenPBR does not define those texture encodings.
6. Once channel semantics are verified, treat every nonconstant authored value, including slight variation, as mapped.
7. Transform color-managed inputs from their declared source color space into the active working color space. Keep data explicitly authored as scalar or vector raw; a scalar destination alone does not make a color image raw.
8. Compare every effective source parameter with the installed Photon node inputs. Record the source expression and its target result as exact, qualitative approximation, or unsupported; do not describe a partial mapping as complete.

## Build Photon graphs

- Author the converted material in its Photon tree (`PH_MATERIAL_NODE_TREE`). Use other renderer trees only as source evidence.
- Prefer a direct mapped Photon input whenever the installed node exposes one. Use a mix approximation only when no closer installed Photon model represents the behavior.
- Inspect live node RNA, socket identifiers, and the installed node class `to_sdl()` behavior. Duplicate socket labels can address different modes, so select by identifier/index plus the controlling mode instead of relying on `inputs.get(name)`. Recreate invalidated nodes and restore links by meaning, not socket index.
- Keep graphs compact and left-to-right: images, component materials, mixes, normal wrapper, output.
- Preserve assignments, masks, emission, normals, and object bindings while replacing the surface.

## Use Binary Mixed Surface correctly

Photon evaluates:

`Material A * Factor + Material B * (1 - Factor)`

- Derive the expected Factor Type from the source control before accepting the Photon node's current mode. Validate expected versus actual mode and active socket; checking only that the current mode's inactive socket is unlinked can certify a consistently wrong graph.
- Put the material selected by factor 1 in A and the material selected by factor 0 in B.
- Set Factor Type to Float and use the float Factor socket for scalar control, including Split, Luminance, and Noise Value outputs. Use Color only when per-channel mixing is authored. Photon exports only the socket selected by Factor Type.
- Extract a verified packed scalar channel with Split Image before using it as a scalar mix factor. Connecting packed RGB directly produces separate per-channel mix factors.

## Convert image values

- Keep maps explicitly authored as scalar data in Raw Data mode and connect them directly. Do not insert Split Image or Luminance without source semantics that require conversion.
- Preserve a color image's declared color-space transform even when it eventually drives a scalar. Cycles implicitly converts Color to Float with scene-linear RGB-to-Y weights; use Photon Luminance only after the source transform and only call it exact when the working primaries match.
- Use Split Image only for verified R/G/B/A packing. A Picture color output connected directly to a Photon scalar consumer reads channel 0; rely on that only when channel 0 is the authored meaning.
- Reuse a Picture node only when its file, color space, Raw Data state, sample mode, and wrap mode are identical for every consumer. Keep separate nodes when any of those semantics differ.
- Materialize a derived sidecar image only when installed nodes or the renderer's file loader cannot represent the source. After rewiring, compare live Picture paths with scoped sidecar files and remove only verified unreferenced or byte-identical resources.

## Lower scalar conversion chains

- Trace through reroutes, Mix, Invert, Map Range, Color Ramp, and math nodes to derive the effective function. Do not bypass a representable intermediary merely because the same texture reaches the destination.
- For a full-factor Invert followed by a linear color-to-scalar conversion, lower it as `1 - scalar(color)` after the required color-space transform.
- A linear Map Range is `to_min + (to_max - to_min) * ((x - from_min) / (from_max - from_min))`; apply Clamp only when enabled. Preserve other interpolation modes as qualitative or unsupported unless Photon can express them.
- A two-stop linear scalar ramp is `y0 + (y1 - y0) * clamp((x - p0) / (p1 - p0), 0, 1)`. A multi-stop linear ramp can be lowered as a sum of clamped segment deltas when every color, constant, and operation is representable. Treat non-linear interpolation and unrepresentable vector math as unsupported.

## Map supported surface behavior

- Matte opaque: use Diffuse Surface, which exports Matte Opaque. Its Albedo input is mapped, and its Roughness input exports an Oren-Nayar sigma map when Oren Nayar is selected.
- Conductive metal: use Abraded Opaque. It maps F0 and isotropic or anisotropic roughness. Use metal/rough base color as F0 only when source semantics define it that way.
- Variable metalness: build complete metal and dielectric branches, put metal in A and dielectric in B, and drive Factor with the extracted metalness map.
- Glossy dielectric over diffuse: if no closer installed Photon node exists, use the qualitative fallback A = white microfacet specular proxy and B = diffuse. For constant source inputs, set the factor to the normal-incidence value `((IOR - 1) / (IOR + 1))^2 * (2 * Specular IOR Level)`; 0.04 is correct only for IOR 1.5 at the default 0.5 level. This still does not reproduce angle-dependent Fresnel or directional energy compensation.
- Perceptual GGX roughness: select `SQUARED` only when the source value is perceptual roughness. Photon implements this option as roughness multiplied by itself; do not apply it to an already converted microfacet alpha.
- Ideal smooth dielectric: set Ideal Substance to `dielectric`, exact Fresnel, and the source IOR. The Blender node defaults to `metallic-reflector` and also exposes mapped reflection and transmission scales.
- Volume-only source: preserve the absence of an opaque surface; never replace an unsupported volume with an opaque proxy. If Photon requires a surface connection, use an Ideal Substance dielectric with equal inside/outside IOR 1 and unit transmission only as a boundary passthrough after verifying geometry and boundary intent. Record every effective volume input as unsupported; the passthrough does not approximate the medium.
- Thin dielectric sheet: use Thin Dielectric Surface when the source calls for a thin sheet. The node exposes thickness, extinction (`Sigma T`), reflection scale, transmission scale, and IOR.
- Rough dielectric interface: use Abraded Translucent for exact or Schlick Fresnel, IOR, and mapped isotropic or anisotropic roughness. It has no thickness, absorption, scattering, or tint inputs; use another supported model or report those source properties.
- Layered coating: use Layered Surface only for behavior its layer inputs encode. Surface Layer exposes roughness, complex IOR, thickness, phase asymmetry, absorption, and scattering, but no diffuse albedo.
- Ambient occlusion: OpenPBR has no AO parameter. Map AO only to a documented Photon input; otherwise preserve and report it rather than silently multiplying base color.

## Map procedural inputs

- Trace only source nodes reachable from the active material output. Do not add a supported procedural node when its only downstream consumer, such as Bump, remains unsupported.
- For Cycles Perlin fBM Noise, map Dimensions directly, Normalize directly, Scale to Frequency, Detail to `Num Layers = Detail + 1`, Roughness to Amplitude Ratio, Lacunarity to Frequency Ratio, and Distortion to Warp. Linked controls require compatible Photon image resources.
- Map Cycles Fac to Photon Value. Photon Noise Color broadcasts the scalar result across color channels; it does not reproduce Cycles' colored Noise output.
- When Photon Coordinates is unlinked, the engine uses hit UVW. An unlinked Cycles Noise Vector uses Generated coordinates. Inspect the complete coordinate chain and every assigned object before calling the result exact; shared materials can require incompatible domains. Document frequency compensation as qualitative if translation, orientation, or phase cannot be preserved.

## Preserve secondary behavior

- Wrap the completed base-surface mixture with Normal Mapped Surface so the wrapper applies to the complete Photon surface.
- Set the normal format from source metadata: `opengl`, `directx`, or `directx-rg`. The node defaults to OpenGL.
- Map constant or linked Normal Map Strength when the source chain is representable. Recreate saved Normal Mapped Surface nodes that lack the current Strength socket, restoring links by meaning plus the original layout and format.
- Connect an alpha-extracted opacity image to Surface Mask. A Picture color output consumed as a scalar reads channel 0, not alpha.
- Treat emission color and strength independently. A linked strength ignores its displayed default; lower the complete strength chain, then multiply the emission color by that result. The current mesh exporter warns that masking emission is unsupported; report that limitation.
- Classify emission per polygon and material slot. An emitter-bearing mesh can contain nonemissive faces; record only faces whose assigned material has a reverse-reachable Surface Emission.
- Audit emitter transform parity, mesh winding, and shading normals separately. A negative-determinant correction does not repair independently backward winding, and a one-sided Photon emitter may differ from a two-sided source material.
- Determine the visible emissive face with a camera/target ray test or near-side geometry, not by choosing whichever normal happens to face the camera; on an inside-out closed mesh, that normal commonly belongs to the occluded far face.
- Photon surface emission tests the shading normal `Ns`. A transform-parity or `should-flip-ng` correction that changes only `Ng` cannot repair inward winding or corner normals. Recalculate connected faces outside only for closed-manifold components proven uniformly inverted; audit open or mixed components face-by-face.
- Before bulk normal repair, create an undo checkpoint and snapshot vertex positions, face material indices, per-loop UV-to-vertex mappings, transforms, and selection/mode. Afterward verify them unchanged, confirm repaired components are outward, and confirm the ray-visible emissive face's transformed `Ns` faces the emission direction with its emission path still reachable.

## Audit and report

For every changed Photon material, verify:

- The output surface is linked and every referenced resource exists.
- The material family matches source metalness, transmission, layering, and surface-versus-volume intent; a volume-only source has no opaque Photon proxy.
- Every behavior-bearing source socket has a recorded linked/default state and effective expression; every varying property is linked and every effective constant matches its source.
- Every new image, math, conversion, and procedural node is reverse-reachable from a used Photon Output input, including surface, emission, and mask. Node presence alone is not validation; remove only nodes proven obsolete and unreachable.
- For every Binary Mix, independently derive expected Factor Type from source semantics, then verify actual mode, active linked factor socket, inactive unlinked factor socket, A/B order, extracted channel, and map.
- Roughness semantics and conversion are correct exactly once.
- Normal format, opacity, emission, color spaces, assignments, and bindings survive conversion.
- Every one-sided emitter has an audited emissive face set; ray-visible faces have transformed `Ns` in the allowed emission hemisphere, and any normal repair preserves per-loop UV-to-vertex mappings and face material indices.
- No stale node schema or link to an inactive duplicate-name socket remains.
- Live Picture paths exist. Deduplicate Picture nodes only when all sampling semantics match and their consumers have been rewired; scoped external material data must contain no newly unreferenced resource.

Keep a per-material ledger with the assigned objects/role, source socket state, effective source expression, Photon mapping, and fidelity. Report exact mappings separately from qualitative approximations and unsupported source behavior; aggregate node counts are not evidence that the material audit is complete.

## Evidence anchors

- Blender MCP architecture and security: <https://github.com/ahujasid/blender-mcp>
- Cycles socket conversion and working-space luminance: <https://projects.blender.org/blender/blender/src/branch/main/intern/cycles/scene/shader_nodes.cpp> and <https://projects.blender.org/blender/blender/src/branch/main/intern/cycles/scene/shader.cpp>
- OpenPBR model and parameter semantics: <https://academysoftwarefoundation.github.io/OpenPBR/>
- Photon tree and socket behavior: `BlenderAddon/PhotonBlend/bmodule/material/node_base.py`
- Binary Mix: `BlenderAddon/PhotonBlend/bmodule/material/surface_nodes/binary_mixed.py`, `Engine/Engine/Source/Engine/Actor/Material/BinaryMixedSurfaceMaterial.h`, and `Engine/Engine/Source/Engine/Core/SurfaceBehavior/SurfaceOptics/TLerpedSurfaceOptics.ipp`
- Matte and microfacet materials: `BlenderAddon/PhotonBlend/bmodule/material/surface_nodes/diffuse.py`, `abraded_opaque.py`, `abraded_translucent.py`, and `Engine/Engine/Source/Engine/Actor/Material/Component/RoughnessToAlphaMapping.h`
- Ideal, thin, layered, and normal-mapped materials: `BlenderAddon/PhotonBlend/bmodule/material/surface_nodes/ideal_substance.py`, `thin_dielectric_surface.py`, `surface_layer.py`, and `normal_mapped.py`
- Picture and conversion nodes: `BlenderAddon/PhotonBlend/bmodule/material/input_nodes/picture.py`, `BlenderAddon/PhotonBlend/bmodule/material/conversion_nodes/split_image.py`, `BlenderAddon/PhotonBlend/bmodule/material/conversion_nodes/luminance.py`, `Engine/Engine/Source/Engine/Actor/Image/SwizzledImage.cpp`, and `Engine/Engine/Source/Engine/Actor/Image/LuminanceImage.cpp`
- Procedural noise and scalar remapping: `BlenderAddon/PhotonBlend/bmodule/material/input_nodes/noise.py`, `BlenderAddon/PhotonBlend/bmodule/material/math_nodes/arithmetic.py`, `BlenderAddon/PhotonBlend/bmodule/material/math_nodes/clamp.py`, `Engine/Engine/Source/Engine/Actor/Image/NoiseImage.cpp`, and `Engine/Engine/Source/Engine/Core/Texture/TFbmNoiseTexture.cpp`
- Mask and emission behavior: `Engine/Engine/Source/Engine/Actor/Image/Image.cpp` and `BlenderAddon/PhotonBlend/bmodule/mesh/export.py`
- Emitter sidedness and normal flipping: `Engine/Engine/Source/Engine/Core/Emitter/SurfaceEmitter.cpp`, `Engine/Engine/Source/Engine/Core/Emitter/DiffuseSurfaceEmitterBase.cpp`, and `Engine/Engine/Source/Engine/Actor/ABlenderPlyModel.h`
