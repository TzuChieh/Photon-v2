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
- Select the color-factor socket when linking a Picture color output. Linked float and color sockets both export an engine factor map; unlinked float constants are replicated across RGB, while unlinked color constants retain their channels.
- Extract a packed scalar channel before using it as a scalar mix factor. Connecting packed RGB directly produces separate per-channel mix factors.
- Photon Engine supports image swizzling, but the Blender add-on's Picture node exposes only a color output. If the installed add-on offers no extraction node, create the required channel-extracted texture beside the active scene or in a scoped subdirectory.

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

## Preserve secondary behavior

- Wrap the completed base-surface mixture with Normal Mapped Surface so the wrapper applies to the complete Photon surface.
- Set the normal format from source metadata: `opengl`, `directx`, or `directx-rg`. The node defaults to OpenGL.
- Connect an alpha-extracted opacity image to Surface Mask. A Picture color output consumed as a scalar reads channel 0, not alpha.
- Connect actual emission maps. The current mesh exporter warns that masking emission is unsupported; report that limitation.

## Audit and report

For every changed Photon material, verify:

- The output surface is linked and every referenced resource exists.
- The material family matches source metalness, transmission, and layering intent.
- Every varying source property is linked and every constant matches its source.
- Binary Mix order, factor socket, extracted channel, and map are correct.
- Roughness semantics and conversion are correct exactly once.
- Normal format, opacity, emission, color spaces, assignments, and bindings survive conversion.
- No stale node schema remains.

Report exact mappings separately from qualitative approximations and unsupported source behavior.

## Evidence anchors

- Blender MCP architecture and security: <https://github.com/ahujasid/blender-mcp>
- OpenPBR model and parameter semantics: <https://academysoftwarefoundation.github.io/OpenPBR/>
- Photon tree and socket behavior: `BlenderAddon/PhotonBlend/bmodule/material/node_base.py`
- Binary Mix: `BlenderAddon/PhotonBlend/bmodule/material/surface_nodes/binary_mixed.py`, `Engine/Engine/Source/Engine/Actor/Material/BinaryMixedSurfaceMaterial.h`, and `Engine/Engine/Source/Engine/Core/SurfaceBehavior/SurfaceOptics/TLerpedSurfaceOptics.ipp`
- Matte and microfacet materials: `BlenderAddon/PhotonBlend/bmodule/material/surface_nodes/diffuse.py`, `abraded_opaque.py`, `abraded_translucent.py`, and `Engine/Engine/Source/Engine/Actor/Material/Component/RoughnessToAlphaMapping.h`
- Ideal, thin, layered, and normal-mapped materials: `BlenderAddon/PhotonBlend/bmodule/material/surface_nodes/ideal_substance.py`, `thin_dielectric_surface.py`, `surface_layer.py`, and `normal_mapped.py`
- Channel, mask, and emission behavior: `BlenderAddon/PhotonBlend/bmodule/material/input_nodes/picture.py`, `Engine/Engine/Source/Engine/Actor/Image/Image.cpp`, `Engine/Engine/Source/Engine/Actor/Image/SwizzledImage.cpp`, and `BlenderAddon/PhotonBlend/bmodule/mesh/export.py`
