
# Main build script for Photon-v2 renderer.

cmake_minimum_required(VERSION 2.8)
project(Photon-v2)

if(WIN32)
    set(BUILD_JNI_DEFAULT ON)
else()
    set(BUILD_JNI_DEFAULT OFF)
endif()

option(BUILD_JNI "Build Photon-v2 JNI for GUI." ${BUILD_JNI_DEFAULT})

# Builds third-party libraries required by Photon-v2.

if(MSVC)

	set(CMAKE_CXX_FLAGS         "${CMAKE_CXX_FLAGS} /std:c++17 /Zi /MP")
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
	set(CMAKE_CXX_FLAGS_DEBUG   "${CMAKE_CXX_FLAGS_DEBUG} /MTd")

else()

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_GLIBCXX_USE_CXX11_ABI=0")

    # compile with c++17 standard enabled
    #
    if(${CMAKE_VERSION} VERSION_LESS "3.8.0")

        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")

    else()

        set(CMAKE_CXX_STANDARD          17)
        set(CMAKE_CXX_STANDARD_REQUIRED ON)

    endif()

endif()


# TODO: add g++ MT MD equivalent flags?

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin/")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin/")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin/")

# HACK: to eliminate extra "Release" folder on MSVC
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${PROJECT_BINARY_DIR}/bin/")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE "${PROJECT_BINARY_DIR}/bin/")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${PROJECT_BINARY_DIR}/bin/")

#get_filename_component(PROJECT_DIR "./" ABSOLUTE)
#set(PROJECT_DIR "${PROJECT_DIR}/")

set(THIRD_PARTY_DIR "${PROJECT_BINARY_DIR}/ThirdParty/")

include(ExternalProject)

#ExternalProject_Add(assimp
#	GIT_REPOSITORY  "https://github.com/assimp/assimp"
#	GIT_TAG         v4.1.0
#	PREFIX          ${THIRD_PARTY_DIR}assimp/
#	CMAKE_ARGS      -DCMAKE_INSTALL_PREFIX=${THIRD_PARTY_DIR}assimp/
#)

if(MSVC)
    set(GOOGLETEST_CXX_FLAGS "/D_SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING")
    #if(CMAKE_BUILD_TYPE MATCHES DEBUG)
    #    #set(GOOGLETEST_CXX_FLAGS "${GOOGLETEST_CXX_FLAGS} /MTd")
    #    set(GOOGLETEST_BUILD_TYPE "${GOOGLETEST_CXX_FLAGS} /MTd")
    #endif()
else()
    set(GOOGLETEST_CXX_FLAGS "")
endif()

ExternalProject_Add(googletest
    GIT_REPOSITORY  "https://github.com/google/googletest"
    GIT_TAG         a868e618c0607259c63f37d948b72586a13922ff
    PREFIX          ${THIRD_PARTY_DIR}googletest/

    CMAKE_ARGS      
    	"-DCMAKE_INSTALL_PREFIX=${THIRD_PARTY_DIR}googletest/"
    	"-DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS} ${GOOGLETEST_CXX_FLAGS}"
)
set(GOOGLETEST_INCLUDE_DIR "${THIRD_PARTY_DIR}googletest/include/")
set(GOOGLETEST_BINARY_DIR  "${THIRD_PARTY_DIR}googletest/lib/")

# a header only library
#
ExternalProject_Add(stb
	GIT_REPOSITORY    "https://github.com/nothings/stb"
	PREFIX            ${THIRD_PARTY_DIR}stb/
	UPDATE_COMMAND    ${GIT_EXECUTABLE} pull
    CONFIGURE_COMMAND ""
    CMAKE_ARGS        ""
    BUILD_COMMAND     ""
    INSTALL_COMMAND   ""
    LOG_DOWNLOAD      ON
)
set(STB_INCLUDE_DIR "${THIRD_PARTY_DIR}stb/src/stb/")

add_subdirectory("./Engine/")
add_subdirectory("./EngineTest/")

if(BUILD_JNI)
    add_subdirectory("./JNI/")
endif()

add_subdirectory("./Application/")


