---
name: blender-artist
description: Author and repair Photon scenes in the currently open Blender instance through blender-mcp while preserving out-of-scope state. Use for Photon object, light, camera, world, or material work and live-scene inspection. Do not use for general non-Photon Blender tasks, offline blend-file rewriting, standalone render comparison, or launching another Blender process.
---

# Blender Artist

## Operating contract

- Use [blender-mcp](https://github.com/ahujasid/blender-mcp) as the scene-authoring interface.
- Verify the intended live scene through MCP before editing: inspect the scene, then confirm `bpy.data.filepath`, scene name, active camera, and render engine in the connected Blender process.
- Perform all scene inspection and mutation through Blender MCP, including short, reviewable in-process Python snippets when needed. Do not write external authoring scripts, rewrite a blend file offline, or launch another Blender instance.
- Stop and report the blocker if Blender MCP cannot reach the intended scene.

## Workflow

1. Define the requested scope and authoritative source data.
2. Inspect relevant live state. Preserve all out-of-scope objects, bindings, world/IBL, cameras, exposure, and settings.
3. For material creation, conversion, repair, or audit, read [the material-authoring reference](references/materials.md) completely before editing. Do not load it for non-material work.
4. Audit material conversions one material at a time. Establish the active source-output path, assigned-object context, and effective linked/default socket values before authoring Photon nodes. Derive expected Photon data types, node modes, and active sockets from that source intent; never use an existing node's selected mode as evidence that the mode is correct.
5. Save or create an undo checkpoint before broad changes or arbitrary code execution.
6. Preflight target identities, schemas, and links before mutation. Author the change through the connected Blender process, then inspect the resulting live state instead of assuming execution succeeded.
7. Save the live scene after validation. In a fresh MCP query, confirm the filepath is unchanged and `bpy.data.is_dirty` is false; save handlers may settle after the save call returns.

## Guardrails

- Place generated outputs in the active scene directory unless the user specifies another destination. If the scene is unsaved, ask for a destination or have the user save it first.
- Discover scene, add-on, and resource paths at runtime. Never hardcode machine-specific paths, and do not assume a remote MCP host shares the local filesystem.
- Do not create helper scripts, notes, or unrelated files unless requested.
- Preserve existing object-material assignments and other bindings unless they are explicitly in scope.
- For sensitive work, review Blender MCP telemetry settings before submitting code or screenshots; disable telemetry when required by the project.
