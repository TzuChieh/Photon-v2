# Photon-v2 Command Line Application

get_filename_component(SDLGEN_CLI_DIR "./" ABSOLUTE)

file(GLOB_RECURSE SDLGEN_CLI_HEADER_FILES 
	"${SDLGEN_CLI_DIR}/Source/*.h"
	"${SDLGEN_CLI_DIR}/Source/*.ipp"
	"${SDLGEN_CLI_DIR}/Source/*.tpp")
file(GLOB_RECURSE SDLGEN_CLI_SOURCE_FILES "${SDLGEN_CLI_DIR}/Source/*.cpp")

# Explicitly mark them as headers
set_source_files_properties(${SDLGEN_CLI_HEADER_FILES} PROPERTIES HEADER_FILE_ONLY ON)

add_executable(SDLGenCLI
	${SDLGEN_CLI_HEADER_FILES}
	${SDLGEN_CLI_SOURCE_FILES})

add_dependencies(SDLGenCLI
	Engine
	SDLGen)

target_include_directories(SDLGenCLI
	PRIVATE ${ENGINE_PRIVATE_INCLUDE_DIR})

target_link_libraries(SDLGenCLI
	PRIVATE Engine
	PRIVATE SDLGen)

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    set_property(TARGET SDLGenCLI 
    	APPEND PROPERTY 
    	LINK_FLAGS /DEBUG:FULL)
endif()

set_target_properties(SDLGenCLI PROPERTIES 
	VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_BINARY_DIR}")

source_group(TREE ${SDLGEN_CLI_DIR} FILES ${SDLGEN_CLI_HEADER_FILES})
source_group(TREE ${SDLGEN_CLI_DIR} FILES ${SDLGEN_CLI_SOURCE_FILES})
