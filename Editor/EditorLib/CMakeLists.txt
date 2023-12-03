# Photon editor library.

get_filename_component(EDITOR_LIB_DIR "./" ABSOLUTE)

set(EDITOR_LIB_INCLUDE_DIR         "${EDITOR_LIB_DIR}/Include/")
set(EDITOR_LIB_PRIVATE_INCLUDE_DIR "${EDITOR_LIB_DIR}/Source/")

set(EDITOR_LIB_CPP_API_INCLUDE_DIRS
	"${ENGINE_PRIVATE_INCLUDE_DIR}"
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
	${IMGUI_LIB_OPENGL3_IMPL_FILES}
	${IMPLOT_LIB_SOURCE_FILES})

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
	PRIVATE ${ENGINE_PRIVATE_INCLUDE_DIR}
	PRIVATE ${IMGUI_LIB_INCLUDE_DIRS}
	PRIVATE ${IMPLOT_LIB_INCLUDE_DIRS})

target_link_libraries(EditorLib
	PUBLIC Engine
	PRIVATE SDLGen
	PRIVATE OpenGL::GL)

include("${CMAKE_SOURCE_DIR}/scripts/cmake/func_link_thirdparty_lib.cmake")

link_thirdparty_lib(EditorLib ${GLFW_LIB_NAME}
	DETECTING_PREPROCESSOR PH_THIRD_PARTY_HAS_GLFW)
link_thirdparty_lib(EditorLib ${GLAD_LIB_NAME}
	DETECTING_PREPROCESSOR PH_THIRD_PARTY_HAS_GLAD)
link_thirdparty_lib(EditorLib ${JSON_LIB_NAME})

# Use 32-bit char type in Dear ImGui. Some font need wider range to encode properly, 
# see https://github.com/ocornut/imgui/blob/master/docs/FONTS.md#using-colorful-glyphsemojis
# This may be defaulted (so can be removed) in a later version of Dear ImGui, 
# see https://github.com/ocornut/imgui/issues/6504#issuecomment-1584661452
target_compile_definitions(EditorLib
	PRIVATE IMGUI_USE_WCHAR32)

source_group(TREE ${EDITOR_LIB_DIR} FILES ${EDITOR_LIB_HEADER_FILES})
source_group(TREE ${EDITOR_LIB_DIR} FILES ${EDITOR_LIB_SOURCE_FILES})

# Make these directories accessible for other sub-projects
set(EDITOR_LIB_CPP_API_INCLUDE_DIRS "${EDITOR_LIB_CPP_API_INCLUDE_DIRS}" PARENT_SCOPE)
