# Photon unit tests for Common library.

get_filename_component(COMMON_LIB_TEST_DIR "./" ABSOLUTE)

file(GLOB_RECURSE COMMON_LIB_TEST_HEADER_FILES "${COMMON_LIB_TEST_DIR}/Source/*.h")
file(GLOB_RECURSE COMMON_LIB_TEST_SOURCE_FILES "${COMMON_LIB_TEST_DIR}/Source/*.cpp")

# Explicitly mark them as headers
set_source_files_properties(${COMMON_LIB_TEST_HEADER_FILES} PROPERTIES HEADER_FILE_ONLY ON)

add_executable(CommonTest ${COMMON_LIB_TEST_HEADER_FILES} ${COMMON_LIB_TEST_SOURCE_FILES})
add_dependencies(CommonTest Common)

target_include_directories(CommonTest
	PRIVATE ${GOOGLETEST_INCLUDE_DIR}
	PRIVATE "${COMMON_LIB_TEST_DIR}/Source/")

target_link_libraries(CommonTest PRIVATE Common)

include("${CMAKE_SOURCE_DIR}/scripts/cmake/func_link_thirdparty_lib.cmake")

link_thirdparty_lib(CommonTest ${GOOGLETEST_LIB_NAME})

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    set_property(TARGET CommonTest
		APPEND PROPERTY 
		LINK_FLAGS /DEBUG:FULL)
		
	# In Visual Studio, Google Test Adapter may need a <filename>.is_google_test 
	# file to automatically detect test cases (when googletest is built as .dll)
	foreach(FILE_DIR ${BUILD_OUTPUT_DIRS})
		set(MARKER_FILE_PATH "${FILE_DIR}/CommonTest.exe.is_google_test")

		file(MAKE_DIRECTORY ${FILE_DIR})
		file(TOUCH          ${MARKER_FILE_PATH})
	endforeach()
endif()

set_target_properties(CommonTest PROPERTIES 
	VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_BINARY_DIR}")

source_group(TREE ${COMMON_LIB_TEST_DIR} FILES ${COMMON_LIB_TEST_HEADER_FILES})
source_group(TREE ${COMMON_LIB_TEST_DIR} FILES ${COMMON_LIB_TEST_SOURCE_FILES})
