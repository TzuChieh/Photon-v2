# Photon unit tests for engine.

get_filename_component(ENGINE_TEST_DIR "./" ABSOLUTE)

file(GLOB_RECURSE ENGINE_TEST_HEADER_FILES "${ENGINE_TEST_DIR}/Source/*.h")
file(GLOB_RECURSE ENGINE_TEST_SOURCE_FILES "${ENGINE_TEST_DIR}/Source/*.cpp")

# Explicitly mark them as engine test headers
set_source_files_properties(${ENGINE_TEST_HEADER_FILES} PROPERTIES HEADER_FILE_ONLY ON)

add_executable(EngineTest ${ENGINE_TEST_HEADER_FILES} ${ENGINE_TEST_SOURCE_FILES})
add_dependencies(EngineTest Engine)

target_include_directories(EngineTest
	PRIVATE ${GOOGLETEST_INCLUDE_DIR}
	PRIVATE "${ENGINE_TEST_DIR}/Source/"
	PRIVATE ${ENGINE_PRIVATE_INCLUDE_DIR})

target_link_libraries(EngineTest PRIVATE Engine)

include("${CMAKE_SOURCE_DIR}/scripts/cmake/func_link_thirdparty_lib.cmake")

link_thirdparty_lib(EngineTest ${GOOGLETEST_LIB_NAME})

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    set_property(TARGET EngineTest
		APPEND PROPERTY 
		LINK_FLAGS /DEBUG:FULL)
		
	# In Visual Studio, Google Test Adapter may need a <filename>.is_google_test 
	# file to automatically detect test cases (when googletest is built as .dll)
	foreach(FILE_DIR ${BUILD_OUTPUT_DIRS})
		set(MARKER_FILE_PATH "${FILE_DIR}/EngineTest.exe.is_google_test")

		file(MAKE_DIRECTORY ${FILE_DIR})
		file(TOUCH          ${MARKER_FILE_PATH})
	endforeach()
endif()

#add_custom_command(TARGET EngineTest
#	POST_BUILD
#	COMMAND ${CMAKE_COMMAND} -E copy_directory
#		"${PROJECT_SOURCE_DIR}/Resource/Test/" 
#		"${PROJECT_BINARY_DIR}/bin/Resource/Test/"
#)

set_target_properties(EngineTest PROPERTIES 
	VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_BINARY_DIR}")

source_group(TREE ${ENGINE_TEST_DIR} FILES ${ENGINE_TEST_HEADER_FILES})
source_group(TREE ${ENGINE_TEST_DIR} FILES ${ENGINE_TEST_SOURCE_FILES})
