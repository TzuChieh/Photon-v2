---
name: photonblend-updater
description: Use this skill when the user wants to update PhotonBlend, Blender add-on SDL bindings, BlenderAddon/PhotonBlend/generated/pysdl.py, or regenerate Blender's Python SDL interface from SDLGen/SDLGenCLI.
---

# PhotonBlend Updater

Read the [Blender Python Versions table](../../../BlenderAddon/README.md#blender-python-versions),
then replace `<python-version>` below with the version bundled with the target Blender. Use the same
version for all build steps sharing a build directory.

If PhotonBlend runtime code consumes `bin/photon_renderer`, clean-build `SDLPyBind` with the matching
Python version:

```shell
python ./scripts/dev_setup_and_build.py --py-ver <python-version> --target SDLPyBind
```

To regenerate `BlenderAddon/PhotonBlend/generated/pysdl.py` and reinstall PhotonBlend:

```shell
python ./scripts/dev_update_blender_addon.py
```

If C++ SDL declarations changed, first do a clean `SDLGenCLI` refresh/build with the matching Python version:

```shell
python ./scripts/dev_setup_and_build.py --py-ver <python-version> --target SDLGenCLI
```

The `photon_renderer` extension is Python-versioned, but its shared `nanobind` library is not. Building
`SDLPyBind` for another Python version in the same output directory can leave Blender's extension next
to an incompatible `nanobind` library. On `DLL load failed while importing photon_renderer`, first
clean-build `SDLPyBind` with the target Blender's Python version and restart Blender; do not add an
export fallback for this build-artifact mismatch.

Do not add `--no-setup` or `--no-cmake`; setup regenerates SDL definition sources. If the user did not
request a build, do not run it and report that `SDLGenCLI` or `SDLPyBind` may be stale.
