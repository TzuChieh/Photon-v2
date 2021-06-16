# Photon-v2 scene description language generation tools.

set(SDL_GEN_NAME "SDLGen")
set(SDL_GEN_CLI_NAME "SDLGenCLI")

get_filename_component(SDL_GEN_DIR "./" ABSOLUTE)

set(SDL_GEN_INCLUDE_DIR         "${SDL_GEN_DIR}/Include/")
set(SDL_GEN_PRIVATE_INCLUDE_DIR "${SDL_GEN_DIR}/Source/")

file(GLOB_RECURSE SDL_GEN_HEADER_FILES 
	"./Include/*.h"
	"./Source/*.h"
	"./Source/*.ipp"
	"./Source/*.tpp"
)
file(GLOB_RECURSE SDL_GEN_SOURCE_FILES "./Source/*.cpp")

# Explicitly mark them as headers
set_source_files_properties(${SDL_GEN_HEADER_FILES} PROPERTIES HEADER_FILE_ONLY ON)

add_library(${SDL_GEN_NAME} ${SDL_GEN_HEADER_FILES} ${SDL_GEN_SOURCE_FILES})
add_dependencies(${SDL_GEN_NAME} Engine)

target_include_directories(${SDL_GEN_NAME}
	PUBLIC  ${SDL_GEN_INCLUDE_DIR}
	PRIVATE ${SDL_GEN_PRIVATE_INCLUDE_DIR}
	PRIVATE ${ENGINE_PRIVATE_INCLUDE_DIR})

target_link_libraries(${SDL_GEN_NAME} PRIVATE Engine)

set(SDL_GEN_NAME        "${SDL_GEN_NAME}"        PARENT_SCOPE)
set(SDL_GEN_INCLUDE_DIR "${SDL_GEN_INCLUDE_DIR}" PARENT_SCOPE)

# TODO: CLI executable
