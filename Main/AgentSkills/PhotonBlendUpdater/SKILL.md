---
name: photonblend-updater
description: Use this skill when the user wants to update PhotonBlend, Blender add-on SDL bindings, BlenderAddon/PhotonBlend/generated/pysdl.py, or regenerate Blender's Python SDL interface from SDLGen/SDLGenCLI.
---

# PhotonBlend Updater

Use the Python version bundled with the target Blender:

| Blender version | Bundled Python version |
| --- | --- |
| Blender 4.5.7 LTS | Python 3.11 |
| Blender 3.6.5 LTS | Python 3.10 |

To regenerate `BlenderAddon/PhotonBlend/generated/pysdl.py` and reinstall PhotonBlend:

```shell
python ./scripts/dev_update_blender_addon.py
```

If C++ SDL declarations changed, first do a clean `SDLGenCLI` refresh/build with the matching Python version:

```shell
python ./scripts/dev_setup_and_build.py --py-ver 3.11 --target SDLGenCLI
```

Do not add `--no-setup` or `--no-cmake`; setup regenerates SDL definition sources. If the user did not request a build, do not run it and report that `SDLGenCLI` may be stale.
