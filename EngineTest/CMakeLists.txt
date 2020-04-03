# Photon-v2 unit tests for engine.

set(ENGINE_TEST_NAME "EngineTest")

file(GLOB_RECURSE SOURCE_FILES "./Source/*.cpp")
add_executable(${ENGINE_TEST_NAME} ${SOURCE_FILES})
add_dependencies(${ENGINE_TEST_NAME} Engine)

target_include_directories(${ENGINE_TEST_NAME}
	PRIVATE ${GOOGLETEST_INCLUDE_DIR}
	PRIVATE "./Source/"
	PRIVATE ${ENGINE_PRIVATE_INCLUDE_DIR}
	PRIVATE ${LIBSIMDPP_INCLUDE_DIR})

target_link_libraries(${ENGINE_TEST_NAME} PRIVATE Engine)

include("${CMAKE_SOURCE_DIR}/scripts/cmake/func_link_thirdparty_lib.cmake")

link_thirdparty_lib(${ENGINE_TEST_NAME} ${GOOGLETEST_LIB_NAME})

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    set_property(TARGET ${ENGINE_TEST_NAME} 
		APPEND PROPERTY 
		LINK_FLAGS /DEBUG:FULL)
		
	# In Visual Studio, Google Test Adapter may need a <filename>.is_google_test 
	# file to automatically detect test cases (when googletest is built as .dll)
	set(ENGINE_TEST_MARKER_FILE_DIR  "${BUILD_OUTPUT_DIR}/")
	set(ENGINE_TEST_MARKER_FILE_PATH "${ENGINE_TEST_MARKER_FILE_DIR}/${ENGINE_TEST_NAME}.exe.is_google_test")
	if(NOT EXISTS "${ENGINE_TEST_MARKER_FILE_PATH}")
		file(MAKE_DIRECTORY "${ENGINE_TEST_MARKER_FILE_DIR}")
		file(TOUCH          "${ENGINE_TEST_MARKER_FILE_PATH}")
	endif()
endif()

#add_custom_command(TARGET ${ENGINE_TEST_NAME}
#	POST_BUILD
#	COMMAND ${CMAKE_COMMAND} -E copy_directory
#		"${PROJECT_SOURCE_DIR}/Resource/Test/" 
#		"${PROJECT_BINARY_DIR}/bin/Resource/Test/"
#)
