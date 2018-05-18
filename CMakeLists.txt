# Main build script for Photon-v2 renderer.

###############################################################################
# General Settings:
#

cmake_minimum_required(VERSION 2.8)
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
# Build Options
#

if(WIN32)
    set(BUILD_JNI_DEFAULT ON)
else()
    set(BUILD_JNI_DEFAULT OFF)
endif()

option(BUILD_JNI "Build Photon-v2 JNI for GUI." ${BUILD_JNI_DEFAULT})

###############################################################################
# Compiler Settings
#

if(MSVC)

    # TODO: more standard ways exist, but requires newer CMake version
    set(CMAKE_CXX_FLAGS         "${CMAKE_CXX_FLAGS} /std:c++17 /Zi /MP")

    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
    set(CMAKE_CXX_FLAGS_DEBUG   "${CMAKE_CXX_FLAGS_DEBUG} /MTd")

else()

    # compiler is GCC
    #
    if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_GLIBCXX_USE_CXX11_ABI=0")
    endif()

    # TODO: G++ assumed here. Do not make this assumption.

    # compile with c++17 standard enabled
    #
    set(CMAKE_CXX_STANDARD          17)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)

    # as a fallback when CMake version < 3.8.0 or whatever reasons
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")

endif()

# TODO: add g++ MT MD equivalent flags?

###############################################################################
# Third-party libraries required by Photon-v2.
#

set(THIRD_PARTY_DIR "${PROJECT_BINARY_DIR}/Photon-v2-ThirdParty/")

if(MSVC)
    set(THIRD_PARTY_PLATFORM "windows")
else()
    set(THIRD_PARTY_PLATFORM "linux")
endif()

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

set(STB_INCLUDE_DIR "${THIRD_PARTY_DIR}stb/include/")

###############################################################################
# Subdirectories:
#

add_subdirectory("./Engine/")
add_subdirectory("./EngineTest/")

if(BUILD_JNI)
    add_subdirectory("./JNI/")
endif()

add_subdirectory("./Application/")


