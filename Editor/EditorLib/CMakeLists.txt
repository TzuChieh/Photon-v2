# Photon editor library.

get_filename_component(EDITOR_LIB_DIR "./" ABSOLUTE)

set(EDITOR_LIB_INCLUDE_DIR         "${EDITOR_LIB_DIR}/Include/")
set(EDITOR_LIB_PRIVATE_INCLUDE_DIR "${EDITOR_LIB_DIR}/Source/")

set(EDITOR_LIB_CPP_API_INCLUDE_DIRS
	"${ENGINE_CPP_API_INCLUDE_DIRS}"
	"${EDITOR_LIB_PRIVATE_INCLUDE_DIR}")

file(GLOB_RECURSE EDITOR_LIB_HEADER_FILES 
	"${EDITOR_LIB_DIR}/Include/*.h"
	"${EDITOR_LIB_DIR}/Source/*.h"
	"${EDITOR_LIB_DIR}/Source/*.ipp"
	"${EDITOR_LIB_DIR}/Source/*.tpp")

file(GLOB_RECURSE EDITOR_LIB_SOURCE_FILES 
	"${EDITOR_LIB_DIR}/Source/*.cpp")

# Explicitly mark them as editor headers
set_source_files_properties(${EDITOR_LIB_HEADER_FILES} PROPERTIES HEADER_FILE_ONLY ON)

add_library(EditorLib
	${EDITOR_LIB_HEADER_FILES}
	${EDITOR_LIB_SOURCE_FILES}
	${IMGUI_LIB_SOURCE_FILES}
	${IMGUI_LIB_GLFW_IMPL_FILES}
	${IMGUI_LIB_OPENGL3_IMPL_FILES})

add_dependencies(EditorLib
	Engine
	SDLGen)

find_package(OpenGL)
if(NOT OPENGL_FOUND)
	message(WARNING 
		"OpenGL not found, the build for Editor will probably fail.")
endif()

target_include_directories(EditorLib
	PUBLIC  ${EDITOR_LIB_INCLUDE_DIR}
	PRIVATE ${EDITOR_LIB_PRIVATE_INCLUDE_DIR}
	PRIVATE ${ENGINE_CPP_API_INCLUDE_DIRS}
	PRIVATE ${IMGUI_LIB_INCLUDE_DIRS})

target_link_libraries(EditorLib
	PUBLIC Engine
	PRIVATE SDLGen
	PRIVATE OpenGL::GL)

include("${CMAKE_SOURCE_DIR}/scripts/cmake/func_link_thirdparty_lib.cmake")

link_thirdparty_lib(EditorLib ${GLFW_LIB_NAME})
link_thirdparty_lib(EditorLib ${GLAD_LIB_NAME})

source_group(TREE ${EDITOR_LIB_DIR} FILES ${EDITOR_LIB_HEADER_FILES})
source_group(TREE ${EDITOR_LIB_DIR} FILES ${EDITOR_LIB_SOURCE_FILES})

# Make these directories accessible for other sub-projects
set(EDITOR_LIB_CPP_API_INCLUDE_DIRS "${EDITOR_LIB_CPP_API_INCLUDE_DIRS}" PARENT_SCOPE)