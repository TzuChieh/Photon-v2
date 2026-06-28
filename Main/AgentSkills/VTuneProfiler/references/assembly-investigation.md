# Assembly Investigation

Use this when a VTune hotspot points at a low-level routine where source changes may or may not improve generated code.

## MSVC Tools

Prefer the Photon-v2 CMake assembly option:

```powershell
python .\scripts\dev_setup_and_build.py --target PhotonCLI -DPH_EMIT_ASSEMBLY=ON
```

`PH_EMIT_ASSEMBLY` is off by default. Enabling it should not change optimized code or runtime performance; it emits extra listing/intermediate files and can slow compilation.

MSVC uses `/FAcs` and Photon-v2 already adds `/Zi` for PDBs. GCC/Clang use `-save-temps=obj` for `.s` files and `-fverbose-asm` for compiler comments.

Visual Studio can also inspect assembly from the command line:

- `cl /FAcs ... /c file.cpp` emits mixed source and assembly beside the object file.
- `dumpbin /DISASM file.obj` or `dumpbin /DISASM file.exe` disassembles an existing binary.
- If `cl` or `dumpbin` is not on `PATH`, locate them under `C:\Program Files\Microsoft Visual Studio\2022\...\VC\Tools\MSVC\...\bin\Hostx64\x64\`.

For templated or heavily inlined code, use build-generated listings first. If the body is still hard to isolate, create a focused translation unit under this skill's ignored `Generated\` directory, instantiate the exact routine through a noinline wrapper, and compile old/new variants with the same flags.

## Before/After Workflow

1. Save the workload, VTune reports, focused source, and assembly under `Main\AgentSkills\VTuneProfiler\Generated\<case>\`.
2. Build an old include overlay from `git show HEAD:path/to/header` or another known baseline when comparing an uncommitted header change.
3. Compile both variants with the same Release-like flags. Prefer `-DPH_EMIT_ASSEMBLY=ON` for project builds; use `/O2 /Ob2 /DNDEBUG /std:c++latest /FAcs` for focused MSVC translation units.
4. Extract only the relevant `PROC` body. Do not compare whole `.asm` files if they contain helper COMDATs.
5. Compare code size, calls, stack frame size, stack-cookie use, spills, dynamic indexed loads, branches, SIMD/scalar instruction count, and loads from temporaries.
6. Treat assembly as supporting evidence. Keep benchmark timing and output correctness checks as the final proof for the real workload.

## Evidence Pattern

Good evidence is specific and tied to the changed hot path. In the AABB hotspot case, the old loop/vector-index version emitted a larger wrapper body with stack materialization, dynamic stack-indexed work, and a stack-cookie check. The scalar x/y/z version emitted a smaller body and removed those stack references, which explained why the compiler did not make both source forms equivalent.

Avoid claiming an optimization is assembly-proven from timing alone, from a disassembly of the wrong configuration, or from helper symbols that are not on the measured call path.
