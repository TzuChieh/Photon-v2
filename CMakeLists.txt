# Main build script for Photon-v2 renderer.

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

set(BUILD_OUTPUT_DIR "${PROJECT_BINARY_DIR}/bin/")
set(THIRDPARTY_DIR   "${PROJECT_BINARY_DIR}/Photon-v2-ThirdParty/")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${BUILD_OUTPUT_DIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${BUILD_OUTPUT_DIR}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${BUILD_OUTPUT_DIR}")

# To eliminate release folder in source directory on MSVC
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${BUILD_OUTPUT_DIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE "${BUILD_OUTPUT_DIR}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${BUILD_OUTPUT_DIR}")

# To eliminate debug folder in source directory on MSVC
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${BUILD_OUTPUT_DIR}/Debug/")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG "${BUILD_OUTPUT_DIR}/Debug/")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${BUILD_OUTPUT_DIR}/Debug/")

#get_filename_component(PROJECT_DIR "./" ABSOLUTE)
#set(PROJECT_DIR "${PROJECT_DIR}/")

# FIXME: cmake doesn't respect CMAKE_PREFIX_PATH for now, seems like a cmake bug
# see https://gitlab.kitware.com/cmake/cmake/issues/18150
# Allow pkg-config take hints from CMAKE_PREFIX_PATH
set(PKG_CONFIG_USE_CMAKE_PREFIX_PATH ON)

# Windows does not come with pkg-config pre-installed, manually locate a prebuilt one
if(WIN32)
    find_program(PKG_CONFIG_EXECUTABLE
        NAMES "pkg-config"
        PATHS "${THIRDPARTY_DIR}/pkg-config-lite/bin/"
        NO_DEFAULT_PATH)

    if(PKG_CONFIG_EXECUTABLE)
        message(STATUS 
            "Found pkg-config.")
    else()
        message(WARNING 
            "pkg-config not found.")
    endif()
else()
# pkg-config on other platforms do not enable "--define-prefix" option
# by default, meaning that prefix variable in .pc files will not be overridden.
# If a library using pkg-config file was relocated (as is the case of using 
# prebuilt libraries), the paths in the config file are no longer valid and
# can cause problems unless we reset it to a proper prefix. However, CMake
# does not provide standard means to do this, which leads to the following
# workaround.
# Ref [1]: https://gitlab.kitware.com/cmake/cmake/-/issues/19254
# Ref [2]: https://stackoverflow.com/questions/52440511/
# Ref [3]: https://www.bassi.io/articles/2018/03/15/pkg-config-and-paths/

    set(PKG_CONFIG_EXECUTABLE "pkg-config --define-prefix")

endif()

###############################################################################
# Build Options                                                               #
###############################################################################

# This is a global flag that affects CMake internal workings
option(BUILD_SHARED_LIBS
    "Build as shared libraries (if applicable)."
    OFF)

set(BUILD_SDL_GEN_DEFAULT       ON)
set(BUILD_PHOTON_CLI_DEFAULT    ON)
set(BUILD_PHOTON_EDITOR_DEFAULT ON)
set(BUILD_ENGINE_TEST_DEFAULT   ON)
set(BUILD_EDITOR_JNI_DEFAULT    OFF)

if(WIN32)
    set(BUILD_EDITOR_JNI_DEFAULT ON)
endif()

option(BUILD_SDL_GEN       "Build SDL automation tool."        ${BUILD_SDL_GEN_DEFAULT})
option(BUILD_PHOTON_CLI    "Build command-line application."   ${BUILD_PHOTON_CLI_DEFAULT})
option(BUILD_PHOTON_EDITOR "Build editor application."         ${BUILD_PHOTON_EDITOR_DEFAULT})
option(BUILD_ENGINE_TEST   "Build unit tests for core engine." ${BUILD_ENGINE_TEST_DEFAULT})
option(BUILD_EDITOR_JNI    "Build JNI for GUI."                ${BUILD_EDITOR_JNI_DEFAULT})

set(THIRD_PARTY_LIBS_INSTALL_DIR 
    "${THIRDPARTY_DIR}/Prebuilt/${THIRD_PARTY_PLATFORM}/release/ThirdParty/" 
    CACHE PATH 
    "Path to third-party library install directory.")

message(STATUS 
    "Third-party library install directory set to \"${THIRD_PARTY_LIBS_INSTALL_DIR}\"; Modify by setting \"THIRD_PARTY_LIBS_INSTALL_DIR\".")

###############################################################################
# Compiler Settings                                                           #
###############################################################################

# Compile with C++17 standard enabled
set(CMAKE_CXX_STANDARD          20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set compiler-dependent settings

# Using Visual Studio C++
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")

    # use C++17 (VS usually do not pick up previous requests)
    set(CMAKE_CXX_FLAGS         "${CMAKE_CXX_FLAGS} /std:c++latest")

    # use PDB files for debugging, which also provides potential optimizations
    set(CMAKE_CXX_FLAGS         "${CMAKE_CXX_FLAGS} /Zi")

    # use multithreaded compilation
    set(CMAKE_CXX_FLAGS         "${CMAKE_CXX_FLAGS} /MP")

    # set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
    # set(CMAKE_CXX_FLAGS_DEBUG   "${CMAKE_CXX_FLAGS_DEBUG} /MTd")

    # VS2019 requires the flag for standard conformant preprocessor; otherwise
    # things may not work correctly (e.g., __VA_ARGS__ will remove preceding 
    # comma on empty argument list)
    # TODO: check on new version, currently required on VS 16.10.4
    # https://devblogs.microsoft.com/cppblog/announcing-full-support-for-a-c-c-conformant-preprocessor-in-msvc/
    set(CMAKE_CXX_FLAGS         "${CMAKE_CXX_FLAGS} /Zc:preprocessor")

# Using GCC
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")

    # as a fallback when CMake version < 3.8.0 or whatever reasons
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")

# Using regular Clang or AppleClang
elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")

    # TODO: enforce C++17 if previous requests have no use

else()
    message(WARNING
        "Unknown compiler ID <${CMAKE_CXX_COMPILER_ID}>, unable to set proper flags.")
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
if(BUILD_ENGINE_TEST)
    set(GOOGLETEST_LIB_NAME "googletest")

    # Bringing in the target "gtest" is unnecessary as "gmock" depends on
    # "gtest" which should be handled by their build system. However, adding
    # "gtest" will result in runtime error due to not calling
    # testing::InitGoogleTest(), which I think is a bug in googletest. See
    # "Photon-v2/EngineTest/Source/main.cpp"
    # for more info.
    
    load_library_via_pkgconfig(${GOOGLETEST_LIB_NAME} OPTIONAL
        TARGETS     "gmock"
        CONFIG_DIR  "${THIRD_PARTY_LIBS_INSTALL_DIR}/${GOOGLETEST_LIB_NAME}/lib/pkgconfig/"
        RUNTIME_DIR "${THIRD_PARTY_LIBS_INSTALL_DIR}/${GOOGLETEST_LIB_NAME}/bin/")

    # Fallback to manual loading
    load_library_manually(${GOOGLETEST_LIB_NAME}
        LIB_NAMES   "gmock"
        LIB_DIRS    "${THIRD_PARTY_LIBS_INSTALL_DIR}/${GOOGLETEST_LIB_NAME}/lib/"
        INC_DIRS    "${THIRD_PARTY_LIBS_INSTALL_DIR}/${GOOGLETEST_LIB_NAME}/include/"
        RUNTIME_DIR "${THIRD_PARTY_LIBS_INSTALL_DIR}/${GOOGLETEST_LIB_NAME}/bin/")
endif()

#------------------------------------------------------------------------------
# stb
#------------------------------------------------------------------------------
set(STB_LIB_NAME "stb")
load_library_via_config(${STB_LIB_NAME}
    TARGETS    "stb"
    CONFIG_DIR "${THIRD_PARTY_LIBS_INSTALL_DIR}/${STB_LIB_NAME}/share/")

#------------------------------------------------------------------------------
# Asio
#------------------------------------------------------------------------------
set(ASIO_LIB_NAME "Asio")
load_library_via_config(${ASIO_LIB_NAME}
    TARGETS    "Asio"
    CONFIG_DIR "${THIRD_PARTY_LIBS_INSTALL_DIR}/${ASIO_LIB_NAME}/share/")

#------------------------------------------------------------------------------
# StackWalker
#------------------------------------------------------------------------------
# TODO: debug build?
# TODO: make this optional

# StackWalker works on Windows only
if(WIN32)
    set(STACKWALKER_LIB_NAME "StackWalker")

    load_library_manually(${STACKWALKER_LIB_NAME}
        LIB_NAMES "StackWalker"
        LIB_DIRS  "${THIRD_PARTY_LIBS_INSTALL_DIR}/${STACKWALKER_LIB_NAME}/lib/"
        INC_DIRS  "${THIRD_PARTY_LIBS_INSTALL_DIR}/${STACKWALKER_LIB_NAME}/include/")
endif()

#------------------------------------------------------------------------------
# zlib
#------------------------------------------------------------------------------

# Note that zlib's .pc file is not suitable for use on Windows (commenting
# out the "prefix" entry and replace the "-lz" link flag with "-lzlib" will
# work, but we avoid the use of package mode on Windows nevertheless).

set(ZLIB_LIB_NAME "zlib")

set(ZLIB_ROOT "${THIRD_PARTY_LIBS_INSTALL_DIR}/${ZLIB_LIB_NAME}/")
find_package(ZLIB MODULE)
if(ZLIB_FOUND)
    load_library_manually(${ZLIB_LIB_NAME}
        TARGETS     "ZLIB::ZLIB"
        RUNTIME_DIR "${THIRD_PARTY_LIBS_INSTALL_DIR}/${ZLIB_LIB_NAME}/bin/")
# Fallback to manual loading
else()
    # FIXME: openexr links to ZLIB::ZLIB, manual loading does not specify that
    load_library_manually(${ZLIB_LIB_NAME}
        INC_DIRS    "${THIRD_PARTY_LIBS_INSTALL_DIR}/${ZLIB_LIB_NAME}/include/"
        LIB_DIRS    "${THIRD_PARTY_LIBS_INSTALL_DIR}/${ZLIB_LIB_NAME}/lib/"
        LIB_NAMES   "zlib z"
        RUNTIME_DIR "${THIRD_PARTY_LIBS_INSTALL_DIR}/${ZLIB_LIB_NAME}/bin/")
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
        CONFIG_DIR  "${THIRD_PARTY_LIBS_INSTALL_DIR}/${OPENEXR_LIB_NAME}/lib/cmake/${ILMBASE_LIB_NAME}/"
        RUNTIME_DIR "${THIRD_PARTY_LIBS_INSTALL_DIR}/${OPENEXR_LIB_NAME}/bin/")

    load_library_via_config(${OPENEXR_LIB_NAME} OPTIONAL
        TARGETS     "IlmImf" "IlmImfUtil"
        CONFIG_DIR  "${THIRD_PARTY_LIBS_INSTALL_DIR}/${OPENEXR_LIB_NAME}/lib/cmake/${OPENEXR_LIB_NAME}/"
        RUNTIME_DIR "${THIRD_PARTY_LIBS_INSTALL_DIR}/${OPENEXR_LIB_NAME}/bin/")
else()
    load_library_via_pkgconfig(${ILMBASE_LIB_NAME} OPTIONAL
        TARGETS     "IlmBase"
        CONFIG_DIR  "${THIRD_PARTY_LIBS_INSTALL_DIR}/${OPENEXR_LIB_NAME}/lib/pkgconfig/"
        RUNTIME_DIR "${THIRD_PARTY_LIBS_INSTALL_DIR}/${OPENEXR_LIB_NAME}/bin/")

    load_library_via_pkgconfig(${OPENEXR_LIB_NAME} OPTIONAL
        TARGETS     "OpenEXR"
        CONFIG_DIR  "${THIRD_PARTY_LIBS_INSTALL_DIR}/${OPENEXR_LIB_NAME}/lib/pkgconfig/"
        RUNTIME_DIR "${THIRD_PARTY_LIBS_INSTALL_DIR}/${OPENEXR_LIB_NAME}/bin/")
endif()

# Fallback to manual loading

load_library_manually(${ILMBASE_LIB_NAME}
    LIB_NAMES   "Half" "Iex" "IexMath" "IlmThread" "Imath"
    LIB_DIRS    "${THIRD_PARTY_LIBS_INSTALL_DIR}/OpenEXR/lib/"
    INC_DIRS    "${THIRD_PARTY_LIBS_INSTALL_DIR}/OpenEXR/include/"
    RUNTIME_DIR "${THIRD_PARTY_LIBS_INSTALL_DIR}/${OPENEXR_LIB_NAME}/bin/")

load_library_manually(${OPENEXR_LIB_NAME}
    LIB_NAMES   "IlmImf" "IlmImfUtil"
    LIB_DIRS    "${THIRD_PARTY_LIBS_INSTALL_DIR}/OpenEXR/lib/"
    INC_DIRS    "${THIRD_PARTY_LIBS_INSTALL_DIR}/OpenEXR/include/"
    RUNTIME_DIR "${THIRD_PARTY_LIBS_INSTALL_DIR}/${OPENEXR_LIB_NAME}/bin/")

#------------------------------------------------------------------------------
# glfw
#------------------------------------------------------------------------------
set(GLFW_LIB_NAME "glfw")

load_library_via_config(${GLFW_LIB_NAME}
    TARGETS      "glfw" NO_PREFIX_ON_TARGETS
    CONFIG_DIR   "${THIRD_PARTY_LIBS_INSTALL_DIR}/glfw/lib/cmake/glfw3/"
    CONFIG_NAMES "glfw3"
    RUNTIME_DIR  "${THIRD_PARTY_LIBS_INSTALL_DIR}/glfw/bin/")

#------------------------------------------------------------------------------
# glad
#------------------------------------------------------------------------------
set(GLAD_LIB_NAME "glad")
load_library_via_config(${GLAD_LIB_NAME}
    TARGETS     "glad"
    CONFIG_DIR  "${THIRD_PARTY_LIBS_INSTALL_DIR}/${GLAD_LIB_NAME}/share/"
    RUNTIME_DIR "${THIRD_PARTY_LIBS_INSTALL_DIR}/glad/bin/")

#------------------------------------------------------------------------------
# imgui
#------------------------------------------------------------------------------
# We do not use imgui the standard way here, as its design is to include the
# library source into your project directly. Several variables are set here
# to facilitate the integration process.

set(IMGUI_LIB_NAME        "imgui")
set(IMGUI_LIB_INSTALL_DIR "${THIRD_PARTY_LIBS_INSTALL_DIR}/${IMGUI_LIB_NAME}/")
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

###############################################################################
# Add CMake Subdirectories                                                    #
###############################################################################

add_subdirectory("./Engine/")

if(BUILD_SDL_GEN)
    add_subdirectory("./SDLGenerator/SDLGen/")
    add_subdirectory("./SDLGenerator/SDLGenCLI/")
endif()

if(BUILD_PHOTON_EDITOR)
    add_subdirectory("./PhotonEditor/")
endif()

if(BUILD_ENGINE_TEST)
    add_subdirectory("./EngineTest/")
endif()

if(BUILD_EDITOR_JNI)
    add_subdirectory("./JNI/")
endif()

if(BUILD_PHOTON_CLI)
    add_subdirectory("./PhotonCLI/")
endif()
