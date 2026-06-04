---
name: binary-updater
description: Use this skill when the user explicitly wants to update, rebuild, or refresh project binaries and generated build outputs. Do not use it for source-only changes or test execution.
---

# Binary Updater

From the repository root, run:

```shell
python ./scripts/dev_setup_and_build.py
```

Use `python ./scripts/dev_setup_and_build.py --help` to see faster paths such as skipping setup/CMake or building one target.

Report completion or the failing command and its error.

If setup fails because Git reports dubious repository ownership, rerun in PowerShell with a process-local override:

```powershell
$env:GIT_CONFIG_COUNT='1'; $env:GIT_CONFIG_KEY_0='safe.directory'; $env:GIT_CONFIG_VALUE_0='D:/dev/Photon-v2'; python .\scripts\dev_setup_and_build.py
```

Do not modify global Git config.
