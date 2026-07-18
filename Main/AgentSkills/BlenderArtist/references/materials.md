# Photon Material Authoring

Read this reference completely only for material creation, conversion, repair, or audit.

## Establish source intent

1. Read the asset documentation and referenced material specification. Inspect raw asset material properties, texture associations, and files.
2. Treat imported Cycles, Principled, or legacy FBX nodes as evidence, not authority, when they conflict with the source. In the Bistro conversion, imported nodes and generic FBX properties did not preserve the source material behavior.
3. Derive channel packing, alpha meaning, normal handedness, and color spaces from authoritative metadata. Do not assume a universal ORM layout, opacity-in-alpha convention, or DirectX/OpenGL normal format; OpenPBR does not define those texture encodings.
4. Once channel semantics are verified, treat every nonconstant authored value, including slight variation, as mapped.
5. Transform color-managed inputs from their declared source color space into the active working color space. Keep scalar and vector data raw.
6. Compare every source parameter with the installed Photon node inputs. Record it as exact, qualitative approximation, or unsupported; do not describe a partial mapping as a complete OpenPBR conversion.

## Build Photon graphs

- Author the converted material in its Photon tree (`PH_MATERIAL_NODE_TREE`). Use other renderer trees only as source evidence.
- Prefer a direct mapped Photon input whenever the installed node exposes one. Use a mix approximation only when no closer installed Photon model represents the behavior.
- Inspect live node RNA, socket identifiers, and the installed node class `to_sdl()` behavior. Recreate invalidated nodes and restore links by meaning, not socket index.
- Keep graphs compact and left-to-right: images, component materials, mixes, normal wrapper, output.
- Preserve assignments, masks, emission, normals, and object bindings while replacing the surface.

## Use Binary Mixed Surface correctly

Photon evaluates:

`Material A * Factor + Material B * (1 - Factor)`

- Put the material selected by factor 1 in A and the material selected by factor 0 in B.
- Set Factor Type to Float and use the float Factor socket for scalar control, including Split, Luminance, and Noise Value outputs. Use Color only when per-channel mixing is authored. Photon exports only the socket selected by Factor Type.
- Extract a verified packed scalar channel with Split Image before using it as a scalar mix factor. Connecting packed RGB directly produces separate per-channel mix factors.

## Convert image values

- Keep dedicated scalar maps as Raw Data and connect them directly. Do not insert Split Image or Luminance without source semantics that require conversion.
- Use Split Image only for verified R/G/B/A packing. Use Luminance for color-to-scalar intent after the declared color-space transform. A Picture color output connected directly to a scalar consumer reads channel 0; rely on that only when channel 0 is the authored meaning.
- Reuse a Picture node only when its file, color space, Raw Data state, sample mode, and wrap mode are identical for every consumer. Keep separate nodes when any of those semantics differ.
- Materialize a derived sidecar image only when installed nodes or the renderer's file loader cannot represent the source. After rewiring, compare live Picture paths with scoped sidecar files and remove only verified unreferenced or byte-identical resources.

## Map supported surface behavior

- Matte opaque: use Diffuse Surface, which exports Matte Opaque. Its Albedo input is mapped, and its Roughness input exports an Oren-Nayar sigma map when Oren Nayar is selected.
- Conductive metal: use Abraded Opaque. It maps F0 and isotropic or anisotropic roughness. Use metal/rough base color as F0 only when source semantics define it that way.
- Variable metalness: build complete metal and dielectric branches, put metal in A and dielectric in B, and drive Factor with the extracted metalness map.
- Glossy dielectric over diffuse: if no closer installed Photon node exists, use the Bistro-tested qualitative fallback A = white microfacet specular proxy and B = diffuse. A factor near 0.04 equals normal-incidence F0 for IOR 1.5, but does not reproduce the dielectric's angle-dependent Fresnel or directional energy compensation.
- Perceptual GGX roughness: select `SQUARED` only when the source value is perceptual roughness. Photon implements this option as roughness multiplied by itself; do not apply it to an already converted microfacet alpha.
- Ideal smooth dielectric: set Ideal Substance to `dielectric`, exact Fresnel, and the source IOR. The Blender node defaults to `metallic-reflector` and also exposes mapped reflection and transmission scales.
- Thin dielectric sheet: use Thin Dielectric Surface when the source calls for a thin sheet. The node exposes thickness, extinction (`Sigma T`), reflection scale, transmission scale, and IOR.
- Rough dielectric interface: use Abraded Translucent for exact or Schlick Fresnel, IOR, and mapped isotropic or anisotropic roughness. It has no thickness, absorption, scattering, or tint inputs; use another supported model or report those source properties.
- Layered coating: use Layered Surface only for behavior its layer inputs encode. Surface Layer exposes roughness, complex IOR, thickness, phase asymmetry, absorption, and scattering, but no diffuse albedo.
- Ambient occlusion: OpenPBR has no AO parameter. Map AO only to a documented Photon input; otherwise preserve and report it rather than silently multiplying base color.

## Map procedural inputs

- Trace only source nodes reachable from the active material output. Do not add a supported procedural node when its only downstream consumer, such as Bump, remains unsupported.
- For Cycles Perlin fBM Noise, map Dimensions directly, Normalize directly, Scale to Frequency, Detail to `Num Layers = Detail + 1`, Roughness to Amplitude Ratio, Lacunarity to Frequency Ratio, and Distortion to Warp. Linked controls require compatible Photon image resources.
- Map Cycles Fac to Photon Value. Photon Noise Color broadcasts the scalar result across color channels; it does not reproduce Cycles' colored Noise output.
- When Photon Coordinates is unlinked, the engine uses hit UVW. An unlinked Cycles Noise Vector uses Generated coordinates. Inspect the complete coordinate chain and every assigned object before calling the result exact; shared materials can require incompatible domains. Document frequency compensation as qualitative if translation, orientation, or phase cannot be preserved.
- Lower a two-stop, linear, black-to-white Color Ramp exactly as `clamp((x - low) / (high - low), 0, 1)` with Subtract, Divide, and Clamp. Preserve arbitrary colors, extra stops, and other interpolation modes as unsupported until a general ramp node exists.

## Preserve secondary behavior

- Wrap the completed base-surface mixture with Normal Mapped Surface so the wrapper applies to the complete Photon surface.
- Set the normal format from source metadata: `opengl`, `directx`, or `directx-rg`. The node defaults to OpenGL.
- Map constant or linked Normal Map Strength when the source chain is representable. Recreate saved Normal Mapped Surface nodes that lack the current Strength socket, restoring links by meaning plus the original layout and format.
- Connect an alpha-extracted opacity image to Surface Mask. A Picture color output consumed as a scalar reads channel 0, not alpha.
- Connect actual emission maps. The current mesh exporter warns that masking emission is unsupported; report that limitation.

## Audit and report

For every changed Photon material, verify:

- The output surface is linked and every referenced resource exists.
- The material family matches source metalness, transmission, and layering intent.
- Every varying source property is linked and every constant matches its source.
- Every new image/math/procedural node is upstream-reachable from Photon Output; node presence alone is not validation.
- Binary Mix order, factor socket, extracted channel, and map are correct.
- Roughness semantics and conversion are correct exactly once.
- Normal format, opacity, emission, color spaces, assignments, and bindings survive conversion.
- No stale node schema remains.
- Live Picture paths exist, and scoped external material data contains no newly unreferenced resource.

Report exact mappings separately from qualitative approximations and unsupported source behavior.

## Evidence anchors

- Blender MCP architecture and security: <https://github.com/ahujasid/blender-mcp>
- OpenPBR model and parameter semantics: <https://academysoftwarefoundation.github.io/OpenPBR/>
- Photon tree and socket behavior: `BlenderAddon/PhotonBlend/bmodule/material/node_base.py`
- Binary Mix: `BlenderAddon/PhotonBlend/bmodule/material/surface_nodes/binary_mixed.py`, `Engine/Engine/Source/Engine/Actor/Material/BinaryMixedSurfaceMaterial.h`, and `Engine/Engine/Source/Engine/Core/SurfaceBehavior/SurfaceOptics/TLerpedSurfaceOptics.ipp`
- Matte and microfacet materials: `BlenderAddon/PhotonBlend/bmodule/material/surface_nodes/diffuse.py`, `abraded_opaque.py`, `abraded_translucent.py`, and `Engine/Engine/Source/Engine/Actor/Material/Component/RoughnessToAlphaMapping.h`
- Ideal, thin, layered, and normal-mapped materials: `BlenderAddon/PhotonBlend/bmodule/material/surface_nodes/ideal_substance.py`, `thin_dielectric_surface.py`, `surface_layer.py`, and `normal_mapped.py`
- Picture and conversion nodes: `BlenderAddon/PhotonBlend/bmodule/material/input_nodes/picture.py`, `BlenderAddon/PhotonBlend/bmodule/material/conversion_nodes/split_image.py`, `BlenderAddon/PhotonBlend/bmodule/material/conversion_nodes/luminance.py`, `Engine/Engine/Source/Engine/Actor/Image/SwizzledImage.cpp`, and `Engine/Engine/Source/Engine/Actor/Image/LuminanceImage.cpp`
- Procedural noise and scalar remapping: `BlenderAddon/PhotonBlend/bmodule/material/input_nodes/noise.py`, `BlenderAddon/PhotonBlend/bmodule/material/math_nodes/arithmetic.py`, `BlenderAddon/PhotonBlend/bmodule/material/math_nodes/clamp.py`, `Engine/Engine/Source/Engine/Actor/Image/NoiseImage.cpp`, and `Engine/Engine/Source/Engine/Core/Texture/TFbmNoiseTexture.cpp`
- Mask and emission behavior: `Engine/Engine/Source/Engine/Actor/Image/Image.cpp` and `BlenderAddon/PhotonBlend/bmodule/mesh/export.py`
