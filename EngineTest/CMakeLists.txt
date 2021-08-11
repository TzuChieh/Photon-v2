# Photon-v2 unit tests for engine.

get_filename_component(ENGINE_TEST_DIR "./" ABSOLUTE)

file(GLOB_RECURSE ENGINE_TEST_HEADER_FILES "./Source/*.h")
file(GLOB_RECURSE ENGINE_TEST_SOURCE_FILES "./Source/*.cpp")

# Explicitly mark them as engine test headers
set_source_files_properties(${ENGINE_TEST_HEADER_FILES} PROPERTIES HEADER_FILE_ONLY ON)

add_executable(EngineTest ${ENGINE_TEST_HEADER_FILES} ${ENGINE_TEST_SOURCE_FILES})
add_dependencies(EngineTest Engine)

target_include_directories(EngineTest
	PRIVATE ${GOOGLETEST_INCLUDE_DIR}
	PRIVATE "./Source/"
	PRIVATE ${ENGINE_PRIVATE_INCLUDE_DIR}
	PRIVATE ${LIBSIMDPP_INCLUDE_DIR})

target_link_libraries(EngineTest PRIVATE Engine)

include("${CMAKE_SOURCE_DIR}/scripts/cmake/func_link_thirdparty_lib.cmake")

link_thirdparty_lib(EngineTest ${GOOGLETEST_LIB_NAME})

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    set_property(TARGET EngineTest
		APPEND PROPERTY 
		LINK_FLAGS /DEBUG:FULL)
		
	# In Visual Studio, Google Test Adapter may need a <filename>.is_google_test 
	# file to automatically detect test cases (when googletest is built as .dll)
	set(ENGINE_TEST_MARKER_FILE_DIR  "${BUILD_OUTPUT_DIR}/")
	set(ENGINE_TEST_MARKER_FILE_PATH "${ENGINE_TEST_MARKER_FILE_DIR}/EngineTest.exe.is_google_test")
	if(NOT EXISTS "${ENGINE_TEST_MARKER_FILE_PATH}")
		file(MAKE_DIRECTORY "${ENGINE_TEST_MARKER_FILE_DIR}")
		file(TOUCH          "${ENGINE_TEST_MARKER_FILE_PATH}")
	endif()
endif()

#add_custom_command(TARGET EngineTest
#	POST_BUILD
#	COMMAND ${CMAKE_COMMAND} -E copy_directory
#		"${PROJECT_SOURCE_DIR}/Resource/Test/" 
#		"${PROJECT_BINARY_DIR}/bin/Resource/Test/"
#)

source_group(TREE ${ENGINE_TEST_DIR} FILES ${ENGINE_TEST_HEADER_FILES})
source_group(TREE ${ENGINE_TEST_DIR} FILES ${ENGINE_TEST_SOURCE_FILES})
