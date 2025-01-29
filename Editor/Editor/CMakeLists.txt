# Photon editor.

get_filename_component(EDITOR_DIR "./" ABSOLUTE)

set(EDITOR_INCLUDE_DIR         "${EDITOR_DIR}/Include/")
set(EDITOR_PRIVATE_INCLUDE_DIR "${EDITOR_DIR}/Source/")

file(GLOB_RECURSE EDITOR_HEADER_FILES 
	"${EDITOR_DIR}/Include/*.h"
	"${EDITOR_DIR}/Source/*.h"
	"${EDITOR_DIR}/Source/*.ipp"
	"${EDITOR_DIR}/Source/*.tpp")

file(GLOB_RECURSE EDITOR_SOURCE_FILES 
	"${EDITOR_DIR}/Source/*.cpp")

# Explicitly mark them as editor headers
set_source_files_properties(${EDITOR_HEADER_FILES} PROPERTIES HEADER_FILE_ONLY ON)

add_executable(Editor
	${EDITOR_HEADER_FILES}
	${EDITOR_SOURCE_FILES})

add_dependencies(Editor
	EditorLib)

target_include_directories(Editor
	PUBLIC  ${EDITOR_INCLUDE_DIR}
	PRIVATE ${EDITOR_PRIVATE_INCLUDE_DIR})

target_link_libraries(Editor
	PRIVATE EditorLib)

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    set_property(TARGET Editor 
    	APPEND PROPERTY 
    	LINK_FLAGS /DEBUG:FULL)
endif()

set_target_properties(Editor PROPERTIES 
	VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_BINARY_DIR}")

source_group(TREE ${EDITOR_DIR} FILES ${EDITOR_HEADER_FILES})
source_group(TREE ${EDITOR_DIR} FILES ${EDITOR_SOURCE_FILES})
