# Main build script for Photon-v2 renderer.

###############################################################################
# General Settings                                                            #
###############################################################################

cmake_policy(SET CMP0054 NEW)

cmake_minimum_required(VERSION 3.1)
project(Photon-v2)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin/")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin/")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin/")

# To eliminate extra "Release" folder on MSVC
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${PROJECT_BINARY_DIR}/bin/")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE "${PROJECT_BINARY_DIR}/bin/")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${PROJECT_BINARY_DIR}/bin/")

#get_filename_component(PROJECT_DIR "./" ABSOLUTE)
#set(PROJECT_DIR "${PROJECT_DIR}/")

###############################################################################
# Build Options                                                               #
###############################################################################

option(BUILD_SHARED_LIBS  "Build as shared libraries if applicable." OFF)

set(BUILD_PHOTON_CLI_DEFAULT ON)

if(WIN32)
    set(BUILD_ENGINE_TEST_DEFAULT ON)
    set(BUILD_EDITOR_JNI_DEFAULT  ON)
else()
    set(BUILD_ENGINE_TEST_DEFAULT OFF)
    set(BUILD_EDITOR_JNI_DEFAULT  OFF)
endif()

option(BUILD_PHOTON_CLI  "Build command-line application."   ${BUILD_PHOTON_CLI_DEFAULT})
option(BUILD_ENGINE_TEST "Build unit tests for core engine." ${BUILD_ENGINE_TEST_DEFAULT})
option(BUILD_EDITOR_JNI  "Build JNI for GUI."                ${BUILD_EDITOR_JNI_DEFAULT})

set(THIRD_PARTY_LIBS_INSTALL_DIR 
    "${PROJECT_BINARY_DIR}/Photon-v2-ThirdParty/build/ThirdParty/" 
    CACHE STRING 
    "Path to third-party library install directory.")

###############################################################################
# Compiler Settings                                                           #
###############################################################################

# compile with C++17 standard enabled
set(CMAKE_CXX_STANDARD          17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# using Visual Studio C++
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")

    set(THIRD_PARTY_PLATFORM "windows")

    # use C++17 (VS usually do not pick up previous requests)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++17")

    # use PDB files for debugging, which also provides potential optimizations
    set(CMAKE_CXX_FLAGS         "${CMAKE_CXX_FLAGS} /Zi")

    # use multithreaded compilation
    set(CMAKE_CXX_FLAGS         "${CMAKE_CXX_FLAGS} /MP")

    # set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
    # set(CMAKE_CXX_FLAGS_DEBUG   "${CMAKE_CXX_FLAGS_DEBUG} /MTd")

# using GCC
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")

    set(THIRD_PARTY_PLATFORM "linux")

    # as a fallback when CMake version < 3.8.0 or whatever reasons
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")

# using regular Clang or AppleClang
elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    
    set(THIRD_PARTY_PLATFORM "osx")

    # TODO: enforce C++17 if previous requests have no use

endif()

# TODO: add g++ MT MD equivalent flags?

###############################################################################
# Gather Third-party Libraries Required by Photon-v2                          #
###############################################################################

include("${CMAKE_SOURCE_DIR}/scripts/cmake/func_load_thirdparty_lib.cmake")

set(THIRD_PARTY_DIR "${PROJECT_BINARY_DIR}/Photon-v2-ThirdParty-master/")

#------------------------------------------------------------------------------
# googletest
#------------------------------------------------------------------------------
if(BUILD_ENGINE_TEST)
    set(GOOGLETEST_LIB_NAME "googletest")

    load_thirdparty_lib(${GOOGLETEST_LIB_NAME} 
        MANUAL_INC_DIRS  "${THIRD_PARTY_LIBS_INSTALL_DIR}/${GOOGLETEST_LIB_NAME}/include/"
        MANUAL_LIB_DIRS  "${THIRD_PARTY_LIBS_INSTALL_DIR}/${GOOGLETEST_LIB_NAME}/lib/"
        MANUAL_LIB_NAMES "gtest" "gmock")

    # load_thirdparty_lib(${GOOGLETEST_LIB_NAME}
    #     MANUAL_INC_DIRS  "${THIRD_PARTY_DIR}/${GOOGLETEST_LIB_NAME}/include/"
    #     MANUAL_LIB_DIRS  "${THIRD_PARTY_DIR}/${GOOGLETEST_LIB_NAME}/lib/${THIRD_PARTY_PLATFORM}/"
    #     MANUAL_LIB_NAMES "gtest" "gmock")
endif()

#------------------------------------------------------------------------------
# stb
#------------------------------------------------------------------------------
set(STB_LIB_NAME "stb")
load_thirdparty_lib(${STB_LIB_NAME}
    CONFIG_PATH     "${THIRD_PARTY_LIBS_INSTALL_DIR}/${STB_LIB_NAME}/share/"
    CONFIG_TARGETS  "stb"
    MANUAL_INC_DIRS "${THIRD_PARTY_DIR}/${STB_LIB_NAME}/include/")

#------------------------------------------------------------------------------
# Asio
#------------------------------------------------------------------------------
set(ASIO_LIB_NAME "Asio")
load_thirdparty_lib(${ASIO_LIB_NAME}
    CONFIG_PATH     "${THIRD_PARTY_LIBS_INSTALL_DIR}/${ASIO_LIB_NAME}/share/"
    CONFIG_TARGETS  "Asio"
    MANUAL_INC_DIRS "${THIRD_PARTY_DIR}/${ASIO_LIB_NAME}/include/")

#------------------------------------------------------------------------------
# StackWalker
#------------------------------------------------------------------------------
# TODO: debug build?
if(WIN32)
    set(STACKWALKER_LIB_NAME "StackWalker")

    load_thirdparty_lib(${STACKWALKER_LIB_NAME} OPTIONAL
        MANUAL_INC_DIRS  "${THIRD_PARTY_LIBS_INSTALL_DIR}/${STACKWALKER_LIB_NAME}/include/"
        MANUAL_LIB_DIRS  "${THIRD_PARTY_LIBS_INSTALL_DIR}/${STACKWALKER_LIB_NAME}/lib/"
        MANUAL_LIB_NAMES "StackWalker")

    load_thirdparty_lib(${STACKWALKER_LIB_NAME}
        MANUAL_INC_DIRS  "${THIRD_PARTY_DIR}/${STACKWALKER_LIB_NAME}/include/"
        MANUAL_LIB_DIRS  "${THIRD_PARTY_DIR}/${STACKWALKER_LIB_NAME}/lib/${THIRD_PARTY_PLATFORM}/"
        MANUAL_LIB_NAMES "StackWalker")
endif()

#------------------------------------------------------------------------------
# zlib
#------------------------------------------------------------------------------
set(ZLIB_LIB_NAME "zlib")
load_thirdparty_lib(${ZLIB_LIB_NAME}
    CONFIG_PATH      "${THIRD_PARTY_LIBS_INSTALL_DIR}/${ZLIB_LIB_NAME}/share/"
    CONFIG_TARGETS   "zlibstatic"
    MANUAL_INC_DIRS  "${THIRD_PARTY_DIR}/${ZLIB_LIB_NAME}/include/"
    MANUAL_LIB_DIRS  "${THIRD_PARTY_DIR}/${ZLIB_LIB_NAME}/lib/${THIRD_PARTY_PLATFORM}/"
    MANUAL_LIB_NAMES "zlibstatic z")

#------------------------------------------------------------------------------
# OpenEXR
#------------------------------------------------------------------------------
if(NOT TARGET Threads::Threads)
    set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
    set(THREADS_PREFER_PTHREAD_FLAG TRUE)
    find_package(Threads)
    if(NOT Threads_FOUND)
        message(FATAL_ERROR 
            "Unable to find a threading library which is required for IlmThread")
    endif()
endif()

set(ILMBASE_LIB_NAME "IlmBase")
set(OPENEXR_LIB_NAME "OpenEXR")

load_thirdparty_lib(${ILMBASE_LIB_NAME}
    CONFIG_PATH      "${THIRD_PARTY_LIBS_INSTALL_DIR}/${OPENEXR_LIB_NAME}/lib/cmake/${ILMBASE_LIB_NAME}/"
    CONFIG_TARGETS   "Half" "Iex" "IexMath" "IlmThread" "Imath"
    MANUAL_INC_DIRS  "${THIRD_PARTY_DIR}/openexr/include/"
    MANUAL_LIB_DIRS  "${THIRD_PARTY_DIR}/openexr/lib/${THIRD_PARTY_PLATFORM}/"
    MANUAL_LIB_NAMES "Half-2_3_s" "Iex-2_3_s" "IexMath-2_3_s" "IlmThread-2_3_s" "Imath-2_3_s")

set(OPENEXR_LIB_NAME "OpenEXR")
load_thirdparty_lib(${OPENEXR_LIB_NAME}
    CONFIG_PATH      "${THIRD_PARTY_LIBS_INSTALL_DIR}/${OPENEXR_LIB_NAME}/lib/cmake/${OPENEXR_LIB_NAME}/"
    CONFIG_TARGETS   "IlmImf" "IlmImfUtil"
    MANUAL_INC_DIRS  "${THIRD_PARTY_DIR}/openexr/include/"
    MANUAL_LIB_DIRS  "${THIRD_PARTY_DIR}/openexr/lib/${THIRD_PARTY_PLATFORM}/"
    MANUAL_LIB_NAMES "IlmImf-2_3_s" "IlmImfUtil-2_3_s")

###############################################################################
# Add CMake Subdirectories                                                    #
###############################################################################

add_subdirectory("./Engine/")

if(BUILD_ENGINE_TEST)
    add_subdirectory("./EngineTest/")
endif()

if(BUILD_EDITOR_JNI)
    add_subdirectory("./JNI/")
endif()

if(BUILD_PHOTON_CLI)
    add_subdirectory("./PhotonCLI/")
endif()
