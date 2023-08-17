# Photon unit tests for editor library.

get_filename_component(EDITOR_TEST_DIR "./" ABSOLUTE)

file(GLOB_RECURSE EDITOR_TEST_HEADER_FILES "${EDITOR_TEST_DIR}/Source/*.h")
file(GLOB_RECURSE EDITOR_TEST_SOURCE_FILES "${EDITOR_TEST_DIR}/Source/*.cpp")

# Explicitly mark them as editor test headers
set_source_files_properties(${EDITOR_TEST_HEADER_FILES} PROPERTIES HEADER_FILE_ONLY ON)

add_executable(EditorTest ${EDITOR_TEST_HEADER_FILES} ${EDITOR_TEST_SOURCE_FILES})
add_dependencies(EditorTest EditorLib)

target_include_directories(EditorTest
	PRIVATE ${GOOGLETEST_INCLUDE_DIR}
	PRIVATE "${EDITOR_TEST_DIR}/Source/"
	PRIVATE ${EDITOR_LIB_CPP_API_INCLUDE_DIRS})

target_link_libraries(EditorTest PRIVATE EditorLib)

include("${CMAKE_SOURCE_DIR}/scripts/cmake/func_link_thirdparty_lib.cmake")

link_thirdparty_lib(EditorTest ${GOOGLETEST_LIB_NAME})

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    set_property(TARGET EditorTest
		APPEND PROPERTY 
		LINK_FLAGS /DEBUG:FULL)
		
	# In Visual Studio, Google Test Adapter may need a <filename>.is_google_test 
	# file to automatically detect test cases (when googletest is built as .dll)
	foreach(FILE_DIR ${BUILD_OUTPUT_DIRS})
		set(MARKER_FILE_PATH "${FILE_DIR}/EditorTest.exe.is_google_test")

		file(MAKE_DIRECTORY ${FILE_DIR})
		file(TOUCH          ${MARKER_FILE_PATH})
	endforeach()
endif()

set_target_properties(EditorTest PROPERTIES 
	VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_BINARY_DIR}")

source_group(TREE ${EDITOR_TEST_DIR} FILES ${EDITOR_TEST_HEADER_FILES})
source_group(TREE ${EDITOR_TEST_DIR} FILES ${EDITOR_TEST_SOURCE_FILES})
