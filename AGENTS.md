# Photon-v2 Guide

Be concise. Local folder `AGENTS.md` rules override this file for their scope.

## Task Routing

- Core renderer: `Engine/Engine`; C API: `Engine/CEngine`; unit tests: `Engine/EngineTest` and
  `Engine/CommonTest`; end-to-end rendering: `Main/RenderTest`.
- Blender integration: `BlenderAddon`; SDL generation/bindings: `SDLInterface`; editor:
  `Editor`; standalone utilities: `Tool`; command-line renderer: `PhotonCLI`.
- Use `Main/Documentation/` for project documentation and
  `Main/Documentation/coding_standard.md` for detailed C++ style.

## Development Conventions

Follow surrounding style and the scoped rules below.

- In hot C++ polymorphic paths, do not assume MSVC devirtualizes unqualified virtual calls on concrete objects or value members; when exact concrete dispatch is intended, use qualified calls and verify speed claims with generated assembly plus repeated workload timing.
- Keep C++ function calls with fewer than three arguments on one line unless it would exceed 100 characters or splitting materially improves clarity.
- Prefix filenames for templated classes with T, including implementation and test files.
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
- Production behavior and generated metadata must not depend on `dev_*` convenience scripts.
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
