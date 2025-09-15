# Photon unit tests for editor library.

get_filename_component(EDITOR_TEST_DIR "./" ABSOLUTE)

set(TARGET_NAME "EditorLibTest")

file(GLOB_RECURSE EDITOR_TEST_HEADER_FILES "${EDITOR_TEST_DIR}/Source/*.h")
file(GLOB_RECURSE EDITOR_TEST_SOURCE_FILES "${EDITOR_TEST_DIR}/Source/*.cpp")

# Explicitly mark them as headers
set_source_files_properties(${EDITOR_TEST_HEADER_FILES} PROPERTIES HEADER_FILE_ONLY ON)

add_executable(${TARGET_NAME} ${EDITOR_TEST_HEADER_FILES} ${EDITOR_TEST_SOURCE_FILES})
add_dependencies(${TARGET_NAME} EditorLib)

target_include_directories(${TARGET_NAME}
	PRIVATE "${EDITOR_TEST_DIR}/Source"
	PRIVATE ${EDITOR_LIB_CPP_API_INCLUDE_DIRS})

target_link_libraries(${TARGET_NAME} PRIVATE EditorLib)

include("${CMAKE_SOURCE_DIR}/scripts/cmake/func_link_thirdparty_lib.cmake")

link_thirdparty_lib(${TARGET_NAME} ${GOOGLETEST_LIB_NAME})

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    set_property(TARGET ${TARGET_NAME}
		APPEND PROPERTY 
		LINK_FLAGS /DEBUG:FULL)
		
	# In Visual Studio, Google Test Adapter may need a <filename>.is_google_test 
	# file to automatically detect test cases (when googletest is built as .dll)
	foreach(FILE_DIR ${BUILD_OUTPUT_DIRS})
		set(MARKER_FILE_PATH "${FILE_DIR}/${TARGET_NAME}.exe.is_google_test")

		file(MAKE_DIRECTORY ${FILE_DIR})
		file(TOUCH          ${MARKER_FILE_PATH})
	endforeach()
endif()

set_target_properties(${TARGET_NAME} PROPERTIES 
	VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_BINARY_DIR}")

source_group(TREE ${EDITOR_TEST_DIR} FILES ${EDITOR_TEST_HEADER_FILES})
source_group(TREE ${EDITOR_TEST_DIR} FILES ${EDITOR_TEST_SOURCE_FILES})
