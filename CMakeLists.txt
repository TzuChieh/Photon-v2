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

# HACK: to eliminate extra "Release" folder on MSVC
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${PROJECT_BINARY_DIR}/bin/")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE "${PROJECT_BINARY_DIR}/bin/")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${PROJECT_BINARY_DIR}/bin/")

#get_filename_component(PROJECT_DIR "./" ABSOLUTE)
#set(PROJECT_DIR "${PROJECT_DIR}/")

###############################################################################
# Build Options                                                               #
###############################################################################

if(WIN32)
    set(BUILD_ENGINE_TEST_DEFAULT ON)
    set(BUILD_EDITOR_JNI_DEFAULT  ON)
else()
    set(BUILD_ENGINE_TEST_DEFAULT OFF)
    set(BUILD_EDITOR_JNI_DEFAULT  OFF)
endif()


option(BUILD_ENGINE_TEST "Build unit tests for core engine." ${BUILD_ENGINE_TEST_DEFAULT})
option(BUILD_EDITOR_JNI  "Build JNI for GUI."                ${BUILD_EDITOR_JNI_DEFAULT})

###############################################################################
# Compiler Settings                                                           #
###############################################################################

# using Visual Studio C++
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")

    set(THIRD_PARTY_PLATFORM "windows")

    # TODO: more standard ways exist, but requires newer CMake version
    set(CMAKE_CXX_FLAGS         "${CMAKE_CXX_FLAGS} /std:c++17 /Zi /MP")

    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
    set(CMAKE_CXX_FLAGS_DEBUG   "${CMAKE_CXX_FLAGS_DEBUG} /MTd")

# using GCC
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")

    set(THIRD_PARTY_PLATFORM "linux")

    if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_GLIBCXX_USE_CXX11_ABI=0")
    endif()

    # compile with c++17 standard enabled
    set(CMAKE_CXX_STANDARD          17)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)

    # as a fallback when CMake version < 3.8.0 or whatever reasons
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")

# using regular Clang or AppleClang
elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    
    set(THIRD_PARTY_PLATFORM "osx")

    # compile with c++17 standard enabled
    set(CMAKE_CXX_STANDARD          17)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)

endif()

# TODO: add g++ MT MD equivalent flags?

###############################################################################
# Gather Third-party Libraries Required by Photon-v2                          #
###############################################################################

set(THIRD_PARTY_DIR "${PROJECT_BINARY_DIR}/Photon-v2-ThirdParty-master/")

# googletest
if(BUILD_ENGINE_TEST)
    set(GOOGLETEST_INCLUDE_DIR "${THIRD_PARTY_DIR}googletest/include/")
    set(GOOGLETEST_BINARY_DIR  "${THIRD_PARTY_DIR}googletest/lib/${THIRD_PARTY_PLATFORM}/")
    find_library(GOOGLETEST_LIB_GTEST gtest
        PATHS ${GOOGLETEST_BINARY_DIR}
        NO_DEFAULT_PATH
    )
    find_library(GOOGLETEST_LIB_GMOCK gmock
        PATHS ${GOOGLETEST_BINARY_DIR}
        NO_DEFAULT_PATH
    )
    set(GOOGLETEST_LIBS "${GOOGLETEST_LIB_GMOCK};${GOOGLETEST_LIB_GTEST}")
endif()

# stb
set(STB_INCLUDE_DIR "${THIRD_PARTY_DIR}stb/include/")

# StackWalker
# TODO: debug build?
if(WIN32)
    set(STACKWALKER_INCLUDE_DIR "${THIRD_PARTY_DIR}StackWalker/include/")
    set(STACKWALKER_BINARY_DIR  "${THIRD_PARTY_DIR}StackWalker/lib/")
    find_library(STACKWALKER_LIB StackWalker
        PATHS ${STACKWALKER_BINARY_DIR}
        NO_DEFAULT_PATH
    )
endif()

# libsimdpp
set(LIBSIMDPP_INCLUDE_DIR "${THIRD_PARTY_DIR}libsimdpp/include/")

# zlib
set(ZLIB_INCLUDE_DIR "${THIRD_PARTY_DIR}zlib/include/")
set(ZLIB_BINARY_DIR  "${THIRD_PARTY_DIR}zlib/lib/${THIRD_PARTY_PLATFORM}/")
find_library(ZLIB_LIB_ZLIB zlibstatic PATHS ${ZLIB_BINARY_DIR} NO_DEFAULT_PATH)
set(ZLIB_LIBS "${ZLIB_LIB_ZLIB}")

# openexr
set(OPENEXR_INCLUDE_DIR "${THIRD_PARTY_DIR}openexr/include/")
set(OPENEXR_BINARY_DIR  "${THIRD_PARTY_DIR}openexr/lib/${THIRD_PARTY_PLATFORM}/")
find_library(OPENEXR_LIB_ILMIMF     IlmImf-2_3_s     PATHS ${OPENEXR_BINARY_DIR} NO_DEFAULT_PATH)
find_library(OPENEXR_LIB_ILMIMFUTIL IlmImfUtil-2_3_s PATHS ${OPENEXR_BINARY_DIR} NO_DEFAULT_PATH)
find_library(OPENEXR_LIB_HALF       Half-2_3_s       PATHS ${OPENEXR_BINARY_DIR} NO_DEFAULT_PATH)
find_library(OPENEXR_LIB_IEX        Iex-2_3_s        PATHS ${OPENEXR_BINARY_DIR} NO_DEFAULT_PATH)
find_library(OPENEXR_LIB_IEXMATH    IexMath-2_3_s    PATHS ${OPENEXR_BINARY_DIR} NO_DEFAULT_PATH)
find_library(OPENEXR_LIB_ILMTHREAD  IlmThread-2_3_s  PATHS ${OPENEXR_BINARY_DIR} NO_DEFAULT_PATH)
find_library(OPENEXR_LIB_IMATH      Imath-2_3_s      PATHS ${OPENEXR_BINARY_DIR} NO_DEFAULT_PATH)
string(CONCAT OPENEXR_LIBS 
    "${OPENEXR_LIB_ILMIMF}"
    ";${OPENEXR_LIB_ILMIMFUTIL}"
    ";${OPENEXR_LIB_HALF}"
    ";${OPENEXR_LIB_IEX}"
    ";${OPENEXR_LIB_IEXMATH}"
    ";${OPENEXR_LIB_ILMTHREAD}"
    ";${OPENEXR_LIB_IMATH}")

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

add_subdirectory("./PhotonCLI/")


