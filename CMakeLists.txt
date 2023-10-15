# Main build script for Photon renderer.

###############################################################################
# General Settings                                                            #
###############################################################################

cmake_minimum_required(VERSION 3.10)

cmake_policy(SET CMP0054 NEW)

if(WIN32)
    set(WINDOWS_SDK_VERSION "10.0" CACHE STRING 
        "The windows SDK version to use. (10.0 = latest)")
    
    set(CMAKE_SYSTEM_VERSION ${WINDOWS_SDK_VERSION} CACHE INTERNAL
        "The version of the operating system for which CMake is to build." FORCE)
    message(STATUS
        "Will build with Windows SDK version ${CMAKE_SYSTEM_VERSION} (10.0 = latest)")
endif()

# Required so that source group calls produce filters in Visual Studio
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

#------------------------------------------------------------------------------
# Main project call is here
#
project(Photon-v2)

message(STATUS
    "Operating System: ${CMAKE_SYSTEM_NAME}")
if(WIN32)
    set(THIRD_PARTY_PLATFORM "windows")
elseif(UNIX AND NOT APPLE)
    set(THIRD_PARTY_PLATFORM "linux")
elseif(APPLE)
    set(THIRD_PARTY_PLATFORM "osx")
else()
    message(WARNING
        "Possibly unsupported operating system detected, assumed to be Linux.")
    set(THIRD_PARTY_PLATFORM "linux")
endif()

set(BUILD_OUTPUT_DIR       "${PROJECT_BINARY_DIR}/bin/")
set(DEBUG_BUILD_OUTPUT_DIR "${BUILD_OUTPUT_DIR}/Debug/")
set(THIRDPARTY_DIR         "${PROJECT_BINARY_DIR}/Photon-v2-ThirdParty/")

set(BUILD_OUTPUT_DIRS
    "${BUILD_OUTPUT_DIR}"
    "${DEBUG_BUILD_OUTPUT_DIR}")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${BUILD_OUTPUT_DIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${BUILD_OUTPUT_DIR}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${BUILD_OUTPUT_DIR}")

# To eliminate release folder in source directory on MSVC
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${BUILD_OUTPUT_DIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE "${BUILD_OUTPUT_DIR}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${BUILD_OUTPUT_DIR}")

# To eliminate debug folder in source directory on MSVC
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${DEBUG_BUILD_OUTPUT_DIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG "${DEBUG_BUILD_OUTPUT_DIR}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${DEBUG_BUILD_OUTPUT_DIR}")

#get_filename_component(PROJECT_DIR "./" ABSOLUTE)
#set(PROJECT_DIR "${PROJECT_DIR}/")

# Windows does not come with pkg-config pre-installed, manually locate a prebuilt one (affects the
# behavior for later find_package(PkgConfig) calls)
if(WIN32)
    find_program(PKG_CONFIG_EXECUTABLE
        NAMES "pkg-config"
        PATHS "${THIRDPARTY_DIR}/Program/pkg-config-lite/bin/"
        NO_DEFAULT_PATH)

    if(PKG_CONFIG_EXECUTABLE)
        message(STATUS 
            "Found bundled pkg-config.")
    else()
        message(WARNING 
            "Bundled pkg-config not found.")
    endif()
endif()

###############################################################################
# Build Options                                                               #
###############################################################################

# This is a global flag that affects CMake internal workings
option(BUILD_SHARED_LIBS
    "Build as shared libraries (if applicable)."
    OFF)

set(BUILD_SDL_GEN_DEFAULT    ON)
set(BUILD_PHOTON_CLI_DEFAULT ON)
set(BUILD_EDITOR_DEFAULT     ON)
set(BUILD_TESTS_DEFAULT      ON)
set(BUILD_EDITOR_JNI_DEFAULT OFF)

if(WIN32)
    set(BUILD_EDITOR_JNI_DEFAULT ON)
endif()

option(PH_BUILD_SDL_GEN     "Build SDL automation tool."        ${BUILD_SDL_GEN_DEFAULT})
option(PH_BUILD_PHOTON_CLI  "Build command-line application."   ${BUILD_PHOTON_CLI_DEFAULT})
option(PH_BUILD_EDITOR      "Build editor application."         ${BUILD_EDITOR_DEFAULT})
option(PH_BUILD_TESTS       "Build unit tests for the project." ${BUILD_TESTS_DEFAULT})
option(PH_BUILD_EDITOR_JNI  "Build JNI for GUI."                ${BUILD_EDITOR_JNI_DEFAULT})

set(PH_THIRD_PARTY_LIBS_INSTALL_DIR 
    "${THIRDPARTY_DIR}/Prebuilt/${THIRD_PARTY_PLATFORM}/ThirdParty/" 
    CACHE PATH 
    "Path to third-party library install directory.")

# For convenience
set(LIBS_INSTALL_DIR "${PH_THIRD_PARTY_LIBS_INSTALL_DIR}")

# For convenience as ImGui libraries are installed under a subfolder
set(IMGUI_LIBS_INSTALL_DIR "${LIBS_INSTALL_DIR}/ImGui/")

message(STATUS 
    "Third-party library install directory set to \"${LIBS_INSTALL_DIR}\"; Modify by setting \"PH_THIRD_PARTY_LIBS_INSTALL_DIR\".")

set(USE_FAST_MATH_DEFAULT OFF)

option(PH_ENABLE_DEBUG "Enable debug functionalities." ON)
option(PH_ENABLE_PROFILING "Enable profiling functionalities." ON)
option(PH_DOUBLE_PRECISION_REAL "Use double precision type for real numbers." OFF)

option(PH_USE_FAST_MATH
    "More room for optimization, may not preserve strict IEEE compliance." 
    ${USE_FAST_MATH_DEFAULT})

###############################################################################
# Compiler Settings                                                           #
###############################################################################

# Set compiler-dependent settings

# Note: We do not use standard CMAKE_CXX_STANDARD and CMAKE_CXX_STANDARD_REQUIRED
# settings since their behavior depend on the CMake version in use. We set 
# appropriate flags manually and only use standard variables as a fallback.

# Using MSVC
if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")

    # Use C++20
    # If this does not work, you should consider using /std:c++latest or other flags
    # (VS does not fully support all C++20 features on VS 2019 v16.11.7)
    #set(CMAKE_CXX_FLAGS         "${CMAKE_CXX_FLAGS} /std:c++20")
    set(CMAKE_CXX_FLAGS         "${CMAKE_CXX_FLAGS} /std:c++latest")

    # HACK: for <= VS 2019 v16.11.7, to make all projects have the /std::c++latest flag
    set(CMAKE_CXX_STANDARD          23)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)

    # Use PDB files for debugging, which also provides potential optimizations
    set(CMAKE_CXX_FLAGS         "${CMAKE_CXX_FLAGS} /Zi")

    # Use multithreaded compilation
    set(CMAKE_CXX_FLAGS         "${CMAKE_CXX_FLAGS} /MP")

    # set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
    # set(CMAKE_CXX_FLAGS_DEBUG   "${CMAKE_CXX_FLAGS_DEBUG} /MTd")

    # VS2019 requires the flag for standard conformant preprocessor; otherwise
    # things may not work correctly (e.g., __VA_ARGS__ will remove preceding 
    # comma on empty argument list)
    # TODO: check on new version, currently required on VS 16.10.4
    # https://devblogs.microsoft.com/cppblog/announcing-full-support-for-a-c-c-conformant-preprocessor-in-msvc/
    set(CMAKE_CXX_FLAGS         "${CMAKE_CXX_FLAGS} /Zc:preprocessor")

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /utf-8")

    if(PH_USE_FAST_MATH)
        add_compile_options(/fp:fast)
    endif()

    # Always add `/bigobj` flag so error C1128 will not occur. This flag
    # generally has no drawback on modern MSVC version (only that generated
    # object file is not compatible to pre-2005 MSVC linker).
    add_compile_options(/bigobj)

# Using gcc/g++
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")

    # Use C++20
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20")

    if(PH_USE_FAST_MATH)
        add_compile_options(-ffast-math)
    endif()

# Using regular Clang or AppleClang (note we use `MATCHES` here)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")

    # TODO: enforce C++20 if previous requests have no use

    if(PH_USE_FAST_MATH)
        add_compile_options(-ffast-math)
    endif()

else()
    message(WARNING
        "Unknown compiler ID <${CMAKE_CXX_COMPILER_ID}>, unable to set specific flags.")

    # Compile with C++20 standard enabled
    set(CMAKE_CXX_STANDARD          20)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
endif()

# TODO: add g++ MT MD equivalent flags?

###############################################################################
# Gather Third-party Libraries Required by Photon-v2                          #
###############################################################################

include("${CMAKE_SOURCE_DIR}/scripts/cmake/func_load_library_via_config.cmake")
include("${CMAKE_SOURCE_DIR}/scripts/cmake/func_load_library_via_pkgconfig.cmake")
include("${CMAKE_SOURCE_DIR}/scripts/cmake/func_load_library_manually.cmake")

#------------------------------------------------------------------------------
# googletest
#------------------------------------------------------------------------------
if(PH_BUILD_TESTS)
    # Bringing in the target "gtest" is unnecessary as "gmock" depends on "gtest" which should be
    # handled by their build system. However, adding "gtest" will result in runtime error due to
    # not calling `testing::InitGoogleTest()`, which I think is a bug in googletest.
    # See "Photon-v2/EngineTest/Source/main.cpp" for more info.
    
    set(GOOGLETEST_LIB_NAME "GTest")
    load_library_via_config(${GOOGLETEST_LIB_NAME} OPTIONAL
        TARGETS      "gmock"
        CONFIG_DIR   "${LIBS_INSTALL_DIR}/googletest/lib/cmake/GTest/"
        RUNTIME_DIR  "${LIBS_INSTALL_DIR}/googletest/bin/")

    if(NOT ${GOOGLETEST_LIB_NAME}_LOADED)
        set(GOOGLETEST_LIB_NAME "googletest")

        load_library_via_pkgconfig(${GOOGLETEST_LIB_NAME} OPTIONAL
            TARGETS     "gmock"
            CONFIG_DIR  "${LIBS_INSTALL_DIR}/googletest/lib/pkgconfig/"
            RUNTIME_DIR "${LIBS_INSTALL_DIR}/googletest/bin/")

        # Fallback to manual loading
        load_library_manually(${GOOGLETEST_LIB_NAME}
            LIB_NAMES   "gmock"
            LIB_DIRS    "${LIBS_INSTALL_DIR}/googletest/lib/"
            INC_DIRS    "${LIBS_INSTALL_DIR}/googletest/include/"
            RUNTIME_DIR "${LIBS_INSTALL_DIR}/googletest/bin/")
    endif()
endif()

#------------------------------------------------------------------------------
# stb
#------------------------------------------------------------------------------
set(STB_LIB_NAME "stb")
load_library_via_config(${STB_LIB_NAME}
    TARGETS    "stb"
    CONFIG_DIR "${LIBS_INSTALL_DIR}/${STB_LIB_NAME}/share/")

#------------------------------------------------------------------------------
# Asio
#------------------------------------------------------------------------------
set(ASIO_LIB_NAME "Asio")
load_library_via_config(${ASIO_LIB_NAME}
    TARGETS    "Asio"
    CONFIG_DIR "${LIBS_INSTALL_DIR}/${ASIO_LIB_NAME}/share/")

#------------------------------------------------------------------------------
# zlib
#------------------------------------------------------------------------------

# Note that zlib's .pc file is not suitable for use on Windows (commenting
# out the "prefix" entry and replace the "-lz" link flag with "-lzlib" will
# work, but we avoid the use of package mode on Windows nevertheless).

set(ZLIB_LIB_NAME "zlib")

set(ZLIB_ROOT "${LIBS_INSTALL_DIR}/${ZLIB_LIB_NAME}/")
find_package(ZLIB MODULE)
if(ZLIB_FOUND)
    load_library_manually(${ZLIB_LIB_NAME}
        TARGETS     "ZLIB::ZLIB"
        RUNTIME_DIR "${LIBS_INSTALL_DIR}/${ZLIB_LIB_NAME}/bin/")
# Fallback to manual loading
else()
    # FIXME: openexr links to ZLIB::ZLIB, manual loading does not specify that
    load_library_manually(${ZLIB_LIB_NAME}
        INC_DIRS    "${LIBS_INSTALL_DIR}/${ZLIB_LIB_NAME}/include/"
        LIB_DIRS    "${LIBS_INSTALL_DIR}/${ZLIB_LIB_NAME}/lib/"
        LIB_NAMES   "zlib z"
        RUNTIME_DIR "${LIBS_INSTALL_DIR}/${ZLIB_LIB_NAME}/bin/")
endif()

#------------------------------------------------------------------------------
# OpenEXR
#------------------------------------------------------------------------------
if(NOT TARGET Threads::Threads)
    set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
    set(THREADS_PREFER_PTHREAD_FLAG TRUE)
    find_package(Threads)
    if(NOT Threads_FOUND)
        message(SEND_ERROR 
            "Unable to find a threading library which is required for IlmThread")
    endif()
endif()

set(ILMBASE_LIB_NAME "IlmBase")
set(OPENEXR_LIB_NAME "OpenEXR")

if(WIN32)
    load_library_via_config(${ILMBASE_LIB_NAME} OPTIONAL
        TARGETS     "Half" "Iex" "IexMath" "IlmThread" "Imath"
        CONFIG_DIR  "${LIBS_INSTALL_DIR}/${OPENEXR_LIB_NAME}/lib/cmake/${ILMBASE_LIB_NAME}/"
        RUNTIME_DIR "${LIBS_INSTALL_DIR}/${OPENEXR_LIB_NAME}/bin/")

    load_library_via_config(${OPENEXR_LIB_NAME} OPTIONAL
        TARGETS     "IlmImf" "IlmImfUtil"
        CONFIG_DIR  "${LIBS_INSTALL_DIR}/${OPENEXR_LIB_NAME}/lib/cmake/${OPENEXR_LIB_NAME}/"
        RUNTIME_DIR "${LIBS_INSTALL_DIR}/${OPENEXR_LIB_NAME}/bin/")
else()
    load_library_via_pkgconfig(${ILMBASE_LIB_NAME} OPTIONAL
        TARGETS     "IlmBase"
        CONFIG_DIR  "${LIBS_INSTALL_DIR}/${OPENEXR_LIB_NAME}/lib/pkgconfig/"
        RUNTIME_DIR "${LIBS_INSTALL_DIR}/${OPENEXR_LIB_NAME}/bin/")

    load_library_via_pkgconfig(${OPENEXR_LIB_NAME} OPTIONAL
        TARGETS     "OpenEXR"
        CONFIG_DIR  "${LIBS_INSTALL_DIR}/${OPENEXR_LIB_NAME}/lib/pkgconfig/"
        RUNTIME_DIR "${LIBS_INSTALL_DIR}/${OPENEXR_LIB_NAME}/bin/")
endif()

# Fallback to manual loading

load_library_manually(${ILMBASE_LIB_NAME}
    LIB_NAMES   "Half" "Iex" "IexMath" "IlmThread" "Imath"
    LIB_DIRS    "${LIBS_INSTALL_DIR}/OpenEXR/lib/"
    INC_DIRS    "${LIBS_INSTALL_DIR}/OpenEXR/include/"
    RUNTIME_DIR "${LIBS_INSTALL_DIR}/${OPENEXR_LIB_NAME}/bin/")

load_library_manually(${OPENEXR_LIB_NAME}
    LIB_NAMES   "IlmImf" "IlmImfUtil"
    LIB_DIRS    "${LIBS_INSTALL_DIR}/OpenEXR/lib/"
    INC_DIRS    "${LIBS_INSTALL_DIR}/OpenEXR/include/"
    RUNTIME_DIR "${LIBS_INSTALL_DIR}/${OPENEXR_LIB_NAME}/bin/")

#------------------------------------------------------------------------------
# glfw
#------------------------------------------------------------------------------
set(GLFW_LIB_NAME "glfw")

load_library_via_config(${GLFW_LIB_NAME}
    TARGETS      "glfw" NO_PREFIX_ON_TARGETS
    CONFIG_DIR   "${LIBS_INSTALL_DIR}/glfw/lib/cmake/glfw3/"
    CONFIG_NAMES "glfw3"
    RUNTIME_DIR  "${LIBS_INSTALL_DIR}/glfw/bin/")

#------------------------------------------------------------------------------
# glad
#------------------------------------------------------------------------------
set(GLAD_LIB_NAME "glad")
load_library_via_config(${GLAD_LIB_NAME}
    TARGETS     "glad"
    CONFIG_DIR  "${LIBS_INSTALL_DIR}/${GLAD_LIB_NAME}/share/"
    RUNTIME_DIR "${LIBS_INSTALL_DIR}/glad/bin/")

#------------------------------------------------------------------------------
# imgui
#------------------------------------------------------------------------------
# We do not use imgui the standard way (as most libraries in Photon) here, as 
# its design is to include the library source into your project directly. 
# Several variables are set here to facilitate the integration process.

set(IMGUI_LIB_NAME        "imgui")
set(IMGUI_LIB_INSTALL_DIR "${IMGUI_LIBS_INSTALL_DIR}/${IMGUI_LIB_NAME}/")
set(IMGUI_LIB_INCLUDE_DIRS
    "${IMGUI_LIB_INSTALL_DIR}/"
    "${IMGUI_LIB_INSTALL_DIR}/backends/")
set(IMGUI_LIB_SOURCE_FILES
    "${IMGUI_LIB_INSTALL_DIR}/imgui.cpp"
    "${IMGUI_LIB_INSTALL_DIR}/imgui_demo.cpp"
    "${IMGUI_LIB_INSTALL_DIR}/imgui_draw.cpp"
    "${IMGUI_LIB_INSTALL_DIR}/imgui_tables.cpp"
    "${IMGUI_LIB_INSTALL_DIR}/imgui_widgets.cpp")
set(IMGUI_LIB_GLFW_IMPL_FILES    "${IMGUI_LIB_INSTALL_DIR}/backends/imgui_impl_glfw.cpp")
set(IMGUI_LIB_OPENGL3_IMPL_FILES "${IMGUI_LIB_INSTALL_DIR}/backends/imgui_impl_opengl3.cpp")

#------------------------------------------------------------------------------
# implot
#------------------------------------------------------------------------------
# Use the same way as imgui.

set(IMPLOT_LIB_NAME        "implot")
set(IMPLOT_LIB_INSTALL_DIR "${IMGUI_LIBS_INSTALL_DIR}/${IMPLOT_LIB_NAME}/")
set(IMPLOT_LIB_INCLUDE_DIRS
    "${IMPLOT_LIB_INSTALL_DIR}/")
set(IMPLOT_LIB_SOURCE_FILES
    "${IMPLOT_LIB_INSTALL_DIR}/implot.cpp"
    "${IMPLOT_LIB_INSTALL_DIR}/implot_demo.cpp"
    "${IMPLOT_LIB_INSTALL_DIR}/implot_items.cpp")

#------------------------------------------------------------------------------
# concurrentqueue
#------------------------------------------------------------------------------
set(CONCURRENTQUEUE_LIB_NAME "concurrentqueue")
load_library_via_config(${CONCURRENTQUEUE_LIB_NAME}
    TARGETS    "concurrentqueue"
    CONFIG_DIR "${LIBS_INSTALL_DIR}/${CONCURRENTQUEUE_LIB_NAME}/lib/cmake/${CONCURRENTQUEUE_LIB_NAME}/")

#------------------------------------------------------------------------------
# json
#------------------------------------------------------------------------------
set(JSON_LIB_NAME "nlohmann_json")
load_library_via_config(${JSON_LIB_NAME}
    TARGETS    "nlohmann_json"
    CONFIG_DIR "${LIBS_INSTALL_DIR}/json/share/cmake/${JSON_LIB_NAME}/")

#------------------------------------------------------------------------------
# tracy
#------------------------------------------------------------------------------
set(TRACY_LIB_NAME "Tracy")
if(PH_ENABLE_PROFILING)
    load_library_via_config(${TRACY_LIB_NAME} OPTIONAL
        TARGETS      "TracyClient"
        CONFIG_DIR   "${LIBS_INSTALL_DIR}/tracy/share/${TRACY_LIB_NAME}/"
        RUNTIME_DIR  "${LIBS_INSTALL_DIR}/tracy/bin/")

    if(NOT ${TRACY_LIB_NAME}_LOADED)
        message(STATUS
            "Tracy not loaded. Tracy-related functionalities will not be available.")
    endif()
endif()
    
###############################################################################
# Add CMake Subdirectories                                                    #
###############################################################################

add_subdirectory("./Engine/")

if(PH_BUILD_SDL_GEN)
    add_subdirectory("./SDLGenerator/SDLGen/")
    add_subdirectory("./SDLGenerator/SDLGenCLI/")
endif()

if(PH_BUILD_EDITOR)
    add_subdirectory("./Editor/EditorLib/")
    add_subdirectory("./Editor/Editor/")
endif()

if(PH_BUILD_TESTS)
    add_subdirectory("./EngineTest/")

    if(PH_BUILD_EDITOR)
        add_subdirectory("./Editor/EditorTest/")
    endif()
endif()

if(PH_BUILD_EDITOR_JNI)
    add_subdirectory("./JNI/")
endif()

if(PH_BUILD_PHOTON_CLI)
    add_subdirectory("./PhotonCLI/")
endif()
