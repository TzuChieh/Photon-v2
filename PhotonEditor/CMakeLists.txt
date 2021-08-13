# Photon-v2 scene description language generation tools.

get_filename_component(PHOTON_EDITOR_DIR "./" ABSOLUTE)

set(PHOTON_EDITOR_INCLUDE_DIR         "${PHOTON_EDITOR_DIR}/Include/")
set(PHOTON_EDITOR_PRIVATE_INCLUDE_DIR "${PHOTON_EDITOR_DIR}/Source/")

file(GLOB_RECURSE PHOTON_EDITOR_HEADER_FILES 
	"${PHOTON_EDITOR_DIR}/Include/*.h"
	"${PHOTON_EDITOR_DIR}/Source/*.h"
	"${PHOTON_EDITOR_DIR}/Source/*.ipp"
	"${PHOTON_EDITOR_DIR}/Source/*.tpp")

file(GLOB_RECURSE PHOTON_EDITOR_SOURCE_FILES 
	"${PHOTON_EDITOR_DIR}/Source/*.cpp")

# Explicitly mark them as editor headers
set_source_files_properties(${PHOTON_EDITOR_HEADER_FILES} PROPERTIES HEADER_FILE_ONLY ON)

add_executable(PhotonEditor
	${PHOTON_EDITOR_HEADER_FILES}
	${PHOTON_EDITOR_SOURCE_FILES}
	${IMGUI_LIB_SOURCE_FILES}
	${IMGUI_LIB_GLFW_IMPL_FILES}
	${IMGUI_LIB_OPENGL3_IMPL_FILES})

add_dependencies(PhotonEditor
	Engine
	SDLGen)

find_package(OpenGL)
if(NOT OPENGL_FOUND)
	message(WARNING 
		"OpenGL not found, the build for Photon Editor will probably fail.")
endif()

target_include_directories(PhotonEditor
	PUBLIC  ${PHOTON_EDITOR_INCLUDE_DIR}
	PRIVATE ${PHOTON_EDITOR_PRIVATE_INCLUDE_DIR}
	PRIVATE ${ENGINE_CPP_API_INCLUDE_DIRS}
	PRIVATE ${IMGUI_LIB_INCLUDE_DIRS})

target_link_libraries(PhotonEditor
	PRIVATE Engine
	PRIVATE SDLGen
	PRIVATE OpenGL::GL)

include("${CMAKE_SOURCE_DIR}/scripts/cmake/func_link_thirdparty_lib.cmake")

link_thirdparty_lib(PhotonEditor ${GLFW_LIB_NAME})
link_thirdparty_lib(PhotonEditor ${GLAD_LIB_NAME})

source_group(TREE ${PHOTON_EDITOR_DIR} FILES ${PHOTON_EDITOR_HEADER_FILES})
source_group(TREE ${PHOTON_EDITOR_DIR} FILES ${PHOTON_EDITOR_SOURCE_FILES})
