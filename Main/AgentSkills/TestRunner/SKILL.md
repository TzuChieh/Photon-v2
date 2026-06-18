---
name: test-runner
description: Agent command guide for clean-building Photon-v2 and running its test suites. Use when Codex is asked to build, refresh binaries, run tests, diagnose test failures, or provide exact build/test commands. Do not use for writing new tests.
---

# Test Runner

For agents running requested validation. Run commands yourself, use clean builds by default, and run tests from `build/` unless noted.

## Build

Clean/full builds can take a long time on Windows; set command timeouts to at least 60 minutes for full build commands before assuming the build is hung.

From repo root:

```powershell
python .\scripts\dev_setup_and_build.py
python .\scripts\dev_setup_and_build.py --target <target>
```

Prefer conservative build parallelism during validation unless the user asks for a faster parallel build.

Use incremental builds only if explicitly requested:

```powershell
python .\scripts\dev_setup_and_build.py --no-setup --no-cmake
python .\scripts\dev_setup_and_build.py --no-setup --no-cmake --target <target>
```

Targets: `CommonTest`, `EngineTest`, `EngineDeepTest`, `EditorLibTest`, `SDLPyBind`.

## C++ Tests

From `build/`:

```powershell
.\bin\CommonTest.exe
.\bin\EngineTest.exe
.\bin\EngineDeepTest.exe
.\bin\EditorLibTest.exe
.\bin\EngineTest.exe --gtest_filter=SuiteName.TestName
```

Mapping: `Engine/Common` -> `CommonTest`; `Engine/Engine` -> `EngineTest`; deep/statistical engine checks -> `EngineDeepTest`; `Editor/EditorLib` -> `EditorLibTest`.

## Python Tests

From `build/`:

```powershell
.\ApplicationEnv\Scripts\python.exe -m pytest .\Test\SDLPyBind
```

The build root `pytest.ini` sets pytest temp output to `Intermediate\pytest`.

## RenderTest

From `build/`, never `Main/RenderTest/`:

```powershell
.\ApplicationEnv\Scripts\python.exe .\RenderTest\run_and_report.py --test-only
.\ApplicationEnv\Scripts\python.exe .\RenderTest\run_and_report.py
.\ApplicationEnv\Scripts\python.exe .\RenderTest\run_and_report.py --report-only
.\ApplicationEnv\Scripts\python.exe .\RenderTest\run_and_report.py --test-only -k unit_radiance
```

After clean setup, verify `build\Photon-v2-Resource\RenderTest` matches the source `Photon-v2-Resource` refs when RenderTest expects newly generated reference names.
