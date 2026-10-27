# Export and Render Validation

Use this for validating scene changes against the running Photon exporter or recovering from a failed preview. Keep scene inspection, export, and mutation in the connected Blender process through MCP.

## Establish the running implementation

- Resolve the loaded PhotonBlend module paths, their real paths or junction targets, and the configured renderer executable through MCP. A newer checkout, an installed add-on, a native render module, and PhotonCLI can be different builds. Check the implementation used by the intended render path before relying on a feature or declaring it unsupported.
- Treat prior review notes as historical evidence. Reconcile them with current graphs and saved authoring decisions; do not reset intentionally tuned light powers or approximations merely because they differ from the original Cycles file.

## Verify evaluated state and exported commands

- Custom Photon properties may change on the original datablock without refreshing the evaluated copy used by export. After editing them, call `update_tag()` on affected node trees, materials, light data, and owning objects as appropriate, then update the view layer/dependency graph. Inspect the evaluated copies before export; a correct original graph alone is insufficient.
- Inspect the actual exported SDL before spending time on a render. Check the changed texture paths and channel settings, material references, coordinates, camera frame, light flags, and sampling settings. If old values remain, resolve the evaluation/export mismatch first.
- Check export diagnostics and representative geometry for material-slot coverage, render UVs, normals, and tangents. An exporter returning successfully does not prove that the intended scene was exported.

## Render checks and persistence

- Before a potentially unstable export or render, save reviewed work when the operating contract's authorization and clean-state conditions permit. An undo checkpoint is only in memory and will not survive a crash. Do not save unrelated pre-existing edits to obtain this protection.
- Snapshot diagnostic settings and temporary modifier or visibility changes, and restore them in `try/finally`. Record whether the dependency graph is `VIEWPORT` or `RENDER`: a viewport export does not automatically include render subdivision, render visibility, or other render-dependent evaluation.
- Verify process completion, logs, image dimensions, finite pixel values, and the image itself. Label a low-sample preview as an export/gross-regression check. Record whether compositing and the intended view transform were applied before comparing appearance with a reference.
- Recheck restored scene settings and bindings, remove only helpers created for the diagnostic, and use a fresh MCP query to verify the final save.

## Diagnose export and render failures

- Inspect logs and crash traces to determine whether failure happened during export, renderer startup, or rendering before retrying. Change authored geometry or materials only when the evidence identifies a scene-data problem.
- If Blender becomes unavailable, stop scene work until MCP reconnects. After restart, re-establish the filepath, dirty state, and saved changes before reapplying anything.
- Verify a repair through the same render entry point and relevant evaluation settings that failed. A manual export or standalone PhotonCLI render can help isolate the problem, but does not prove that F12 works. Report diagnostic workarounds separately from verified repairs.
