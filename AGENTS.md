# Project Context for AI Agents

When working on this project, you are allowed to sacrifice grammar for concision.

## Project Overview

Photon-v2 is a physically based rendering engine written primarily in C++23. It aims to provide a toolset for ultra-realistic image synthesis. The project is cross-platform (Windows, Linux, and macOS). It features its own scene description language (`.p2` files) and includes a Blender add-on for scene creation and export.

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
* Under `PhotonBlend/`
  - A Blender add-on for scene creation, material editing, and rendering within Blender.
* Under `SDLInterface/`
  - `SDLGen` is for generating SDL textual commands and documentation. `SDLPyBind` is for generating Python bindings for functions exposed by SDL.
* Under `Tools/`
  - `FilmMerger` is for combinding different render outputs.

The JNI (Java Native Interface) portion of the project is deprecated.

## Project Documentation

You can find the main entries in `Main/Documentation/`.

## Development Conventions

Follow the style of existing/surrounding code. Favor code with better quality and less prone to human errors. If specific rules are needed, the project has a detailed C++ coding standard in `Main/Documentation/coding_standard.md`.

## Git

- **Stage Before Update:** Always stage existing changes (`git add -A`) before applying new updates or corrections to the code. This ensures the user can see the "delta" (diff) in their IDE (like VSCode) between the staged (old) and unstaged (new) versions.
- **Read-Only by Default:** Only perform read-only git operations. Write operations are only allowed if requested by the human user.

## Code Review

When asked to review a change, mention the most critical issue first. Other issues can be described as a one-liner and only elaborate on request. 
