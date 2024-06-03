# Build From Source

[TOC]

Building Photon is super easy since the renderer is mostly coded from scratch using standard C++. Each of the required third-party libraries is either header only or pre-compiled, and can be handled automatically with the setup script.

## Prerequisites

Photon uses CMake as its main build system, and the toolkits you need are as follows:

* CMake 3.10+ (3.20+ recommended)
* Python 3 (3.9 recommended)
* C++20 compliant compiler (necessary)

If you want to build the editor in previous version, you will also need:

* JDK 1.8+ and Maven

This is not required for building the new C++ based editor.

*Note that the working directory is assumed to be the project root (in the folder that you cloned) if not stated, and please use `./build/` as build folder for now since build scripts are more or less hard-coded to use this path, currently.*

## Step 1: Run the Setup Script

Run the setup script (depending on your system, choose either `./setup.bat` or `./setup.sh`), this will download pre-compiled libraries and resources into "./build".

## Step 2: Compile

For Windows, just use CMake-GUI to generate project files for your favorite IDE.

For Linux and macOS, run the following commands:

* `cd ./build/`
* `cmake -DCMAKE_BUILD_TYPE=release ../`
* `make`

## Step 3: Have Fun

The compiled binaries will be in the `./build/bin/` folder.

## Appendix A: Available CMake Options

Several options are available for fine-tuning the built binaries. To specify a CMake option named `SOME_OPTION` with value `SOME_VALUE`, you should add it as an additional argument in the form

```shell
cmake -DSOME_OPTION=SOME_VALUE (other arguments...)
```

and substitute `SOME_OPTION` and `SOME_VALUE` with the options listed below.

*(do not forget the -D prefix)*

| Options          | Values        | Effects  |
| -------------    | ------------- | ----- |
| CMAKE_BUILD_TYPE | release, debug | When set, build binaries with optimizations enabled; otherwise no optimization is done. |
| PH_BUILD_TESTS | ON (default), OFF | Build unit tests. They should be executed from the build folder. |
| PH_BUILD_EDITOR_JNI | ON, OFF (default) | Build JNI for Photon Studio (previous editor). |
| PH_BUILD_EDITOR | ON (default), OFF | Build editor application. |
| PH_BUILD_SDL_GEN | ON (default), OFF | Build SDL automation tool. |
| PH_BUILD_PHOTON_CLI | ON (default), OFF | Build command-line application. |

You can also obtain a complete list of options with descriptions by running `cmake -LAH ../` in the build folder.

## Appendix B: Building Documentation

Similar to building the libraries, running setup script is also required (depending on your system, choose either `./setup.bat` or `./setup.sh`). After that, you can either choose to run Doxygen from the same directory as the `doxygen.config` file of each project, or run `python ./scripts/build_doxygen_doc.py <path-to-doxygen-executable>`. The output will be in `./Main/docs/`.

You will also need the `bibtex` tool to be present in the search path to generate project bibliography.
