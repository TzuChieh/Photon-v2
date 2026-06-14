---
name: vtune-profiler
description: Guide for profiling Photon-v2 native executables with Intel VTune. Use when Hotspots/call-stack reports, vtune.exe setup, symbol diagnosis, or assembly checks are needed. Do not use for ordinary tests or binary refreshes.
---

# VTune Profiler

Profile an existing Photon-v2 native executable with Intel VTune and generate text/CSV reports.

In VTune, collection runs the target under `vtune -collect` and writes raw result data; report generation reads that result and writes text/CSV summaries.

Do not build Photon-v2 unless explicitly asked. If needed, use the project build script:

```powershell
python .\scripts\dev_setup_and_build.py --target PhotonCLI
```

## Inputs

VTune needs a target executable and workload args. For `PhotonCLI`, pass the scene/workload arguments used to reproduce the slow path. The script does not infer workloads.

Ask step by step when missing:

- VTune path: ask for `vtune.exe` or installation.
- Target: ask for the existing executable to profile; do not assume a build directory.
- Workload: ask for the scene/config/CLI args that reproduce the slow path.

## Local Settings

Optional machine-local settings live in ignored `local.settings.json`, validated by `local.settings.schema.json`. Example default-build values:

```json
{
  "vtune_exe": "C:/Program Files (x86)/Intel/oneAPI/vtune/latest/bin64/vtune.exe",
  "search_dirs": ["build/bin", "."]
}
```

Adjust `search_dirs` if the local build directory differs. The script also checks `--vtune`, this file's `vtune_exe`, `VTUNE_EXE`, `PATH`, and common oneAPI install locations, in that order.

Do not store secrets in the skill directory.

## Run

Example using the default PhotonCLI output layout; adjust `--target` if the local build directory differs:

```powershell
python .\Main\AgentSkills\VTuneProfiler\scripts\run_vtune_profile.py `
  --target build\bin\PhotonCLI.exe `
  --target-arg=-s `
  --target-arg scenes\fractal.p2 `
  --target-arg=-o `
  --target-arg Main\AgentSkills\VTuneProfiler\Generated\fractal-vtune
```

Results and reports default to ignored `Main\AgentSkills\VTuneProfiler\Generated\`. For target args beginning with `-`, use `--target-arg=-flag` form. Pass plain VTune search directories with `--search-dir`, e.g. the actual build output directory and repo root.

## Reading Results

Start with the Hotspots report, then inspect call paths in VTune GUI or with `vtune -report callstacks` if top functions are tiny helpers. Optimize clusters, not isolated leaf names; for Photon render workloads, expect ray/geometry intersection clusters such as BVH traversal, AABB tests, triangle tests, transforms, and vector math.

For low-level hotspot changes, confirm codegen with assembly before trusting timings alone. See `references/assembly-investigation.md`.

## Symbols

Photon-v2 MSVC builds already set:

- top-level CMake adds `/Zi`;
- `PhotonCLI` and C++ test executables add `/DEBUG:FULL`;
- `dev_setup_and_build.py` builds Release by default.

If VTune shows addresses or `[Unknown]`, check the exact profiled `.exe`/`.dll`, matching `.pdb`, then add plain binary/source directories with `--search-dir`. Profile executables, not static libraries.
