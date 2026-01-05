# Photon scene description language generation tool (CLI).

get_filename_component(SDL_GEN_CLI_DIR "./" ABSOLUTE)

set(TARGET_NAME "SDLGenCLI")

file(GLOB_RECURSE SDL_GEN_CLI_HEADER_FILES 
	"${SDL_GEN_CLI_DIR}/Source/*.h"
	"${SDL_GEN_CLI_DIR}/Source/*.ipp"
	"${SDL_GEN_CLI_DIR}/Source/*.tpp")
file(GLOB_RECURSE SDL_GEN_CLI_SOURCE_FILES "${SDL_GEN_CLI_DIR}/Source/*.cpp")

# Explicitly mark them as headers
set_source_files_properties(${SDL_GEN_CLI_HEADER_FILES} PROPERTIES HEADER_FILE_ONLY ON)

add_executable(${TARGET_NAME}
	${SDL_GEN_CLI_HEADER_FILES}
	${SDL_GEN_CLI_SOURCE_FILES})

add_dependencies(${TARGET_NAME}
	Engine
	SDLGen)

target_include_directories(${TARGET_NAME}
	PRIVATE "${SDL_GEN_CLI_DIR}/Source"
	PRIVATE ${ENGINE_PRIVATE_INCLUDE_DIR})

target_link_libraries(${TARGET_NAME}
	PRIVATE Engine
	PRIVATE SDLGen)

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    set_property(TARGET ${TARGET_NAME} 
    	APPEND PROPERTY 
    	LINK_FLAGS /DEBUG:FULL)
endif()

set_target_properties(${TARGET_NAME} PROPERTIES 
	VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_BINARY_DIR}")

source_group(TREE ${SDL_GEN_CLI_DIR} FILES ${SDL_GEN_CLI_HEADER_FILES})
source_group(TREE ${SDL_GEN_CLI_DIR} FILES ${SDL_GEN_CLI_SOURCE_FILES})
