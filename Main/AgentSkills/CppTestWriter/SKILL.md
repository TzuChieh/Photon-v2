---
name: cpp-test-writer
description: Use this skill when the user wants to create, repair, extend, or assess focused C++ unit tests and test coverage using local Google Test style.
---

# C++ Test Writer

Write focused tests whose contracts and expected results are obvious at a glance.

## Workflow

### 1. Map Context

- Read scoped `AGENTS.md`, the public API and documentation, and 2-3 nearby tests for local style.
- Focus on the requested public behavior and shared interfaces.

### 2. Design Coverage

- Test only general behavior promised or directly implied by the public API. Include realistic
  corner cases a caller can derive from that API; do not encode private implementation states or
  speculative cases.
- Derive expected behavior from the public contract before inspecting implementation details. Do
  not repeat implementation logic in assertions.
- Prefer direct inputs and expected values whose correctness is easy to verify by inspection. Use
  helpers only when they remove noise without hiding the behavior under test.
- Group variants of one contract in a single test using scoped blocks. Use a separate test only for
  distinct behavior, not another input variant.
- Ask the user only when ambiguity prevents a reliable expected result.

### 3. Implement and Check

- Match nearby Google Test structure, include ordering, naming, and formatting.
- Add new test data to the separate `Photon-v2-Resource` repo, not only the ignored
  `build/Photon-v2-Resource` setup copy. Ask the user for the source repo location when needed.
- Add comments only when they clarify a non-obvious derivation.
- Review the diff and run `git diff --check`. Do not build or run tests; report the command the user
  should run when useful.

## Project Test Mapping

- `Engine/Common` -> `Engine/CommonTest`
- `Engine/Engine` -> `Engine/EngineTest`
- `Editor/EditorLib` -> `Editor/EditorLibTest`
