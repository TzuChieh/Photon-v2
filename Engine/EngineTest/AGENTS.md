# EngineTest Guide

## Generated Files
- Use `EngineTestIntermediatePath` and `Filesystem` for temporary outputs. Isolate each test under `<suite>/<test>`, remove that directory before the test, and leave end-of-test cleanup out unless the test requires it.

## Cooking Tests
- Prefer direct resource/context setup over bootstrapping `VisualWorld` when a unit test only needs cooked-resource allocation.
