# Photon scene description language generation tools.

get_filename_component(SDL_GEN_DIR "./" ABSOLUTE)

set(TARGET_NAME                     SDLGen)
set(SDL_GEN_INCLUDE_DIR         "${SDL_GEN_DIR}/Include")
set(SDL_GEN_PRIVATE_INCLUDE_DIR "${SDL_GEN_DIR}/Source")

file(GLOB_RECURSE SDL_GEN_HEADER_FILES 
	"${SDL_GEN_DIR}/Include/${TARGET_NAME}/*.h"
	"${SDL_GEN_DIR}/Source/${TARGET_NAME}/*.h"
	"${SDL_GEN_DIR}/Source/${TARGET_NAME}/*.ipp"
	"${SDL_GEN_DIR}/Source/${TARGET_NAME}/*.tpp"
)
file(GLOB_RECURSE SDL_GEN_SOURCE_FILES "${SDL_GEN_DIR}/Source/${TARGET_NAME}/*.cpp")

# Explicitly mark them as headers
set_source_files_properties(${SDL_GEN_HEADER_FILES} PROPERTIES HEADER_FILE_ONLY ON)

add_library(${TARGET_NAME} ${SDL_GEN_HEADER_FILES} ${SDL_GEN_SOURCE_FILES})
add_dependencies(${TARGET_NAME} Engine)

target_include_directories(${TARGET_NAME}
	PUBLIC  ${SDL_GEN_INCLUDE_DIR}
	PRIVATE ${SDL_GEN_PRIVATE_INCLUDE_DIR}
	PRIVATE ${ENGINE_PRIVATE_INCLUDE_DIR})

target_link_libraries(${TARGET_NAME} PRIVATE Engine)

source_group(TREE ${SDL_GEN_DIR} FILES ${SDL_GEN_HEADER_FILES})
source_group(TREE ${SDL_GEN_DIR} FILES ${SDL_GEN_SOURCE_FILES})

set(SDL_GEN_INCLUDE_DIR "${SDL_GEN_INCLUDE_DIR}" PARENT_SCOPE)
