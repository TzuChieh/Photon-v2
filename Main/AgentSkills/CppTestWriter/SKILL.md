---
name: cpp-test-writer
description: Use this skill when the user wants to create, repair, extend, or assess focused C++ unit tests and test coverage using local Google Test style.
---

# C++ Test Writer

Write focused tests with independently understandable expected results.

## Workflow

### 1. Map Context

- Read scoped `AGENTS.md`, the public API and documentation, and 2-3 nearby tests for local style.
- Focus on the requested behavior. Otherwise prioritize public contracts and shared interfaces.

### 2. Design Coverage

- Derive expected behavior from the public contract before inspecting implementation details. Do not repeat implementation logic in assertions.
- Prefer tests that are simple, minimal, and straightforward. Each test should prove one useful contract without extra setup or incidental assertions.
- Cover normal behavior and relevant edge, error, or lifetime cases with stable, controlled inputs. Inspect implementation afterward only to identify missed risks and branches.
- Ask the user only when ambiguity prevents a reliable expected result.

### 3. Implement and Check

- Match nearby Google Test structure, include ordering, naming, and formatting.
- Add new test data to the separate `Photon-v2-Resource` repo, not only the ignored `build/Photon-v2-Resource` setup copy. Ask the user for the source repo location when needed.
- Add comments only when they clarify a non-obvious derivation.
- Review the diff and run `git diff --check`. Do not build or run tests; report the command the user should run when useful.

## Project Test Mapping

- `Engine/Common` -> `Engine/CommonTest`
- `Engine/Engine` -> `Engine/EngineTest`
- `Editor/EditorLib` -> `Editor/EditorLibTest`
