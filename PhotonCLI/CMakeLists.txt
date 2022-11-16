# Photon-v2 Command Line Application

get_filename_component(PHOTON_CLI_DIR "./" ABSOLUTE)

file(GLOB_RECURSE PHOTON_CLI_HEADER_FILES 
	"${PHOTON_CLI_DIR}/*.h"
	"${PHOTON_CLI_DIR}/*.ipp"
	"${PHOTON_CLI_DIR}/*.tpp")

file(GLOB_RECURSE PHOTON_CLI_SOURCE_FILES 
	"${PHOTON_CLI_DIR}/*.cpp")

# Explicitly mark them as headers
set_source_files_properties(${PHOTON_CLI_HEADER_FILES} PROPERTIES HEADER_FILE_ONLY ON)

add_executable(PhotonCLI ${PHOTON_CLI_HEADER_FILES} ${PHOTON_CLI_SOURCE_FILES})

add_dependencies(PhotonCLI
	Engine
	SDLGen)

target_include_directories(PhotonCLI
	PRIVATE ${ENGINE_CPP_API_INCLUDE_DIRS})

target_link_libraries(PhotonCLI
	PRIVATE Engine
	PRIVATE SDLGen)

include("${CMAKE_SOURCE_DIR}/scripts/cmake/func_link_thirdparty_lib.cmake")

link_thirdparty_lib(PhotonCLI ${ASIO_LIB_NAME})

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    set_property(TARGET PhotonCLI 
    	APPEND PROPERTY 
    	LINK_FLAGS /DEBUG:FULL)
endif()

set_target_properties(PhotonCLI PROPERTIES 
	VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_BINARY_DIR}")

source_group(TREE ${PHOTON_CLI_DIR} FILES ${PHOTON_CLI_HEADER_FILES})
source_group(TREE ${PHOTON_CLI_DIR} FILES ${PHOTON_CLI_SOURCE_FILES})
