# Photon-v2 scene description language generation tools.

set(PHOTON_EDITOR_NAME "PhotonEditor")

get_filename_component(PHOTON_EDITOR_DIR "./" ABSOLUTE)

set(PHOTON_EDITOR_INCLUDE_DIR         "${PHOTON_EDITOR_DIR}/Include/")
set(PHOTON_EDITOR_PRIVATE_INCLUDE_DIR "${PHOTON_EDITOR_DIR}/Source/")

file(GLOB_RECURSE PHOTON_EDITOR_HEADER_FILES 
	"./Include/*.h"
	"./Source/*.h"
	"./Source/*.ipp"
	"./Source/*.tpp"
)
file(GLOB_RECURSE PHOTON_EDITOR_SOURCE_FILES "./Source/*.cpp")

# Explicitly mark them as editor headers
set_source_files_properties(${PHOTON_EDITOR_HEADER_FILES} PROPERTIES HEADER_FILE_ONLY ON)

add_executable(${PHOTON_EDITOR_NAME} ${PHOTON_EDITOR_HEADER_FILES} ${PHOTON_EDITOR_SOURCE_FILES})
add_dependencies(${PHOTON_EDITOR_NAME} Engine)
add_dependencies(${PHOTON_EDITOR_NAME} ${SDL_GEN_NAME})

target_include_directories(${PHOTON_EDITOR_NAME}
	PUBLIC  ${PHOTON_EDITOR_INCLUDE_DIR}
	PRIVATE ${PHOTON_EDITOR_PRIVATE_INCLUDE_DIR}
	PRIVATE ${ENGINE_PRIVATE_INCLUDE_DIR})

target_link_libraries(${PHOTON_EDITOR_NAME}
	PRIVATE Engine
	PRIVATE ${SDL_GEN_NAME})
