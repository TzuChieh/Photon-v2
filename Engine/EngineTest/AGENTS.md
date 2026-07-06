# EngineTest Guide

## Test Discovery
- When adding or renaming `Source/*.cpp` tests, rerun CMake/setup before expecting `EngineTest` to discover them; the target uses CMake `GLOB_RECURSE`, not runtime test-file discovery.

## Generated Files
- Use `EngineTestIntermediatePath` and `Filesystem` for temporary outputs. Isolate each test under `<suite>/<test>`, remove that directory before the test, and leave end-of-test cleanup out unless the test requires it.

## Cooking Tests
- Prefer direct resource/context setup over bootstrapping `VisualWorld` when a unit test only needs cooked-resource allocation.
- Keep material/cooking unit tests focused on engine-level contracts that are easy to break locally; avoid duplicating behavior already exercised end-to-end by RenderTest.

## Fixture Design
- For compact mappings or transformed data, choose fixtures where source count, unique value count, and encoded output count differ so the storage contract is tested.
