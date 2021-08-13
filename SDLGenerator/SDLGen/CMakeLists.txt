# Photon-v2 scene description language generation tools.

get_filename_component(SDL_GEN_DIR "./" ABSOLUTE)

set(SDL_GEN_INCLUDE_DIR         "${SDL_GEN_DIR}/Include/")
set(SDL_GEN_PRIVATE_INCLUDE_DIR "${SDL_GEN_DIR}/Source/")

file(GLOB_RECURSE SDL_GEN_HEADER_FILES 
	"${SDL_GEN_DIR}/Include/*.h"
	"${SDL_GEN_DIR}/Source/*.h"
	"${SDL_GEN_DIR}/Source/*.ipp"
	"${SDL_GEN_DIR}/Source/*.tpp"
)
file(GLOB_RECURSE SDL_GEN_SOURCE_FILES "${SDL_GEN_DIR}/Source/*.cpp")

# Explicitly mark them as headers
set_source_files_properties(${SDL_GEN_HEADER_FILES} PROPERTIES HEADER_FILE_ONLY ON)

add_library(SDLGen ${SDL_GEN_HEADER_FILES} ${SDL_GEN_SOURCE_FILES})
add_dependencies(SDLGen Engine)

target_include_directories(SDLGen
	PUBLIC  ${SDL_GEN_INCLUDE_DIR}
	PRIVATE ${SDL_GEN_PRIVATE_INCLUDE_DIR}
	PRIVATE ${ENGINE_CPP_API_INCLUDE_DIRS})

target_link_libraries(SDLGen PRIVATE Engine)

source_group(TREE ${SDL_GEN_DIR} FILES ${SDL_GEN_HEADER_FILES})
source_group(TREE ${SDL_GEN_DIR} FILES ${SDL_GEN_SOURCE_FILES})

set(SDL_GEN_INCLUDE_DIR "${SDL_GEN_INCLUDE_DIR}" PARENT_SCOPE)
