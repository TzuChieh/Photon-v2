# Project Structure {#main_project_structure}

[TOC]

This page contains an overview of the project's structure for you to quickly find the documentation or source code that you need. Photon is not a single library, it is composed of multiple libraries and programs. Together, they form a toolset for ultra realistic image synthesis.

## Programs

* Editor: GUI for scene editing, rendering and debugging
  - [Source Code](https://github.com/TzuChieh/Photon-v2/tree/master/Editor/Editor)
* PhotonCLI: Command-line interface of the renderer
  - [Documentation](../../PhotonCLI/html/index.html)
  - [Source Code](https://github.com/TzuChieh/Photon-v2/tree/master/PhotonCLI)
* SDLGenCLI: Command-line interface for generating Photon Scene Description Language (PSDL) bindings
  - [Source Code](https://github.com/TzuChieh/Photon-v2/tree/master/SDLGenerator/SDLGenCLI)
* FilmMerger: Combine the output from multiple rendering sessions
  - [Documentation](../../FilmMerger/html/index.html)
  - [Source Code](https://github.com/TzuChieh/Photon-v2/tree/master/Tool/FilmMerger)
* IntersectError: A small internal tool for estimating the accuracy of different ray-primitive intersection strategies
  - [Source Code](https://github.com/TzuChieh/Photon-v2/tree/master/Tool/IntersectError)

## Plugins

* PhotonBlend: Blender Add-on for Photon
  - [Documentation](../../PhotonBlend/html/index.html)
  - [Source Code](https://github.com/TzuChieh/Photon-v2/tree/master/BlenderAddon)

## Libraries

* Common: Common functionalities, definitions and utilities for Photon projects
  - [Documentation](../../Common/html/index.html)
  - [Source Code](https://github.com/TzuChieh/Photon-v2/tree/master/Engine/Common)
* CEngine: C API of the render engine
  - [Documentation](../../CEngine/html/index.html)
  - [Source Code](https://github.com/TzuChieh/Photon-v2/tree/master/Engine/CEngine)
* Engine: Rendering algorithms and core functionalities
  - [Documentation](../../Engine/html/index.html)
  - [Source Code](https://github.com/TzuChieh/Photon-v2/tree/master/Engine/Engine)
* EditorLib: Core editor functionalities of the renderer
  - [Documentation](../../EditorLib/html/index.html)
  - [Source Code](https://github.com/TzuChieh/Photon-v2/tree/master/Editor/EditorLib)
* SDLGen: Core of SDLGenCLI
  - [Source Code](https://github.com/TzuChieh/Photon-v2/tree/master/SDLGenerator/SDLGen)

## Tests

* EngineTest: Unit tests for the engine
  - [Source Code](https://github.com/TzuChieh/Photon-v2/tree/master/Engine/EngineTest)
* EditorTest: Unit tests for the editor
  - [Source Code](https://github.com/TzuChieh/Photon-v2/tree/master/Editor/EditorTest)
* RenderTest: End-to-end tests for the renderer
  - [Example Report](https://github.com/TzuChieh/Photon-v2-TestReport/blob/main/RenderTest/2024_03_11/703c352/report.md)
  - [Reports](https://github.com/TzuChieh/Photon-v2-TestReport/tree/main/RenderTest)
  - [Documentation](./RenderTest/README.md)
  - [Source Code](https://github.com/TzuChieh/Photon-v2/tree/master/Main/RenderTest)

## Miscellaneous

* Example Scenes: Simple scenes for testing out the renderer
  - [Scenes](https://github.com/TzuChieh/Photon-v2/tree/master/scenes)
* Scripts: Various scripts for setup and development
  - [Source Code](https://github.com/TzuChieh/Photon-v2/tree/master/scripts)
* Resource: Additional resources for Photon
  - [Repository](https://github.com/TzuChieh/Photon-v2-Resource)
* ThirdParty: Customized third-party libraries for Photon
  - [Repository](https://github.com/TzuChieh/Photon-v2-ThirdParty)

## Deprecated Modules

* JNI: Java bindings for the engine
  - [Source Code](https://github.com/TzuChieh/Photon-v2/tree/master/JNI)
* PhotonStudio: GUI for scene editing, rendering and debugging, with various utilities (e.g., Minecraft map parser, PSDL Java bindings, etc.)
  - [Source Code](https://github.com/TzuChieh/Photon-v2/tree/master/PhotonStudio)
