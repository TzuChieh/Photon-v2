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
  - `FilmMerger` is for combining different render outputs.

## Project Documentation

You can find the main entries in `Main/Documentation/`.

## Development Conventions

Follow the style of existing/surrounding code. Favor code with better quality and less prone to human errors. If specific rules are needed, the project has a detailed C++ coding standard in `Main/Documentation/coding_standard.md`.

## Rules
- Do not build or run binaries/tests; user handles execution unless they explicitly request a binary refresh. For binary refreshes, use the `binary-updater` skill.
- JNI/PhotonStudio and `scripts/SDL_Interface/` paths are deprecated; avoid expanding legacy usage unless explicitly requested.
- Keep file line endings consistent with the current OS convention.

## Git Rules
- Read-only git by default.
- Do not stage changes unless user explicitly asks.
- One explicit LGTM allows one commit only.

## External Resources
- Setup downloads `Photon-v2-Resource` into the ignored `build/Photon-v2-Resource/` copy via `scripts/resource_downloader.py`.
- Add or update test fixtures and render scenes in the separate `Photon-v2-Resource` source repo, not only the build copy. Ask the user for its local path when needed.

## Scoped Guidance
- Under `Engine/`: C API contracts, filesystem paths, engine tests, SDL internals, geometry actors, intersection primitives/data structures, and renderer-specific details.
- Under `Main/`: end-to-end render tests and project-local skills in `Main/AgentSkills/`.
