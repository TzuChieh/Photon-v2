# Photon-v2 Guide

Be concise. Local folder `AGENTS.md` rules override this file for their scope.

## Project Overview

Photon-v2 is a physically based rendering engine written primarily in C++23. It aims to provide a toolset for ultra-realistic image synthesis. The project is cross-platform (Windows, Linux, and macOS). It features its own scene description language  and includes a Blender add-on for scene creation and export.

## Project Structure

The project is organized into several libraries and applications:

* Under `Engine`
  - `Engine` is the core rendering engine, containing core functionalities and rendering algorithms. `EngineTest` is `Engine`'s unit test. `Common` is a support library with common utilities and definitions used across the project. `CommonTest` is `Common`'s unit test. `CEngine` is a C-style API for the rendering engine.
* Under `Editor`
  - `Editor` is a graphical user interface for scene editing and rendering, while `EditorLib` is its actual internals and `EditorLibTest` is the unit test.
* Under `Main/RenderTest/`
  - End-to-end rendering tests.
* Under `PhotonCLI/`
  - A command-line interface for the renderer.
* Under `BlenderAddon/`
  - A Blender add-on for scene creation, material editing, and rendering within Blender.
* Under `SDLInterface/`
  - `SDLGen` is for generating SDL textual commands and documentation. `SDLPyBind` is for generating Python bindings for functions exposed by SDL.
* Under `Tool/`
  - `FilmMerger` combines different render outputs. `IntersectError` estimates ray-primitive intersection accuracy.

## Project Documentation

You can find the main entries in `Main/Documentation/`.

## Development Conventions

Follow the style of existing/surrounding code. Favor code with better quality and less prone to human errors. If specific rules are needed, the project has a detailed C++ coding standard in `Main/Documentation/coding_standard.md`.
- In hot C++ polymorphic paths, do not assume MSVC devirtualizes unqualified virtual calls on concrete objects or value members; when exact concrete dispatch is intended, use qualified calls and verify speed claims with generated assembly plus repeated workload timing.
- Keep C++ function calls with fewer than three arguments on one line unless it would exceed 100 characters or splitting materially improves clarity.
- Do not add `static_cast` for identity or clearly non-narrowing standard conversions. Keep
  explicit casts for narrowing, signedness or precision changes, enums, pointer downcasts or
  cross-casts, user-defined conversions, and overload selection.
- Choose C++ helper placement by clarity and ownership: keep a one-off operation at its call site
  when clearer, use a private member when it primarily uses one class's state, and use an anonymous
  file-local helper for class-independent implementation details.
- Keep comments concise: state a contract or non-obvious reason without restating the code.
- Never remove existing comments unless the user explicitly asks; update them in place when surrounding behavior changes.

## Rules
- Do not build or run binaries/tests unless requested; use `test-runner` with `python .\scripts\dev_setup_and_build.py` for validation, and `binary-updater` for binary refreshes.
- For build validation, use `--no-setup --no-cmake` when dependencies and CMake config are current; full configure/build runs may need unsandboxed MSVC access.
- JNI/PhotonStudio and `scripts/SDL_Interface/` paths are deprecated; avoid expanding legacy usage unless explicitly requested.
- Keep file line endings consistent with the current OS convention. Must be CRLF on Windows.

## Git Rules
- Read-only git by default.
- Never stage or unstage paths unless the user explicitly asks; preserve the existing index state.
- One explicit LGTM allows one commit only.

## External Resources
- Setup downloads `Photon-v2-Resource` into the ignored `build/Photon-v2-Resource/` copy via `scripts/resource_downloader.py`.
- Add or update test fixtures and render scenes in the separate `Photon-v2-Resource` source repo, not only the build copy. Ask the user for its local path when needed.

## Scoped Guidance
- Under `Engine/`: C API contracts, filesystem paths, engine tests, SDL internals, geometry actors, intersection primitives/data structures, and renderer-specific details.
- Under `Main/`: end-to-end render tests and project-local skills in `Main/AgentSkills/`.
