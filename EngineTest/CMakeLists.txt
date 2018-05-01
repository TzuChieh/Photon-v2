
# Photon-v2 unit tests for engine.

#set(CMAKE_CXX_FLAGS         "${CMAKE_CXX_FLAGS} /std:c++17 /Zi /MP /MT")
#set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
#set(CMAKE_CXX_FLAGS_DEBUG   "${CMAKE_CXX_FLAGS_DEBUG} /MT")

file(GLOB_RECURSE SOURCE_FILES "./Source/*.cpp")
add_executable(EngineTest ${SOURCE_FILES})

add_dependencies(EngineTest Engine)
add_dependencies(EngineTest googletest)

target_include_directories(EngineTest 
	PRIVATE "./Source/"
	PRIVATE ${ENGINE_INCLUDE_DIR}
	PRIVATE ${ENGINE_PRIVATE_INCLUDE_DIR}
	PRIVATE ${GOOGLETEST_INCLUDE_DIR}
)

set(GMOCK_LIB_NAME ${CMAKE_STATIC_LIBRARY_PREFIX}gmock${CMAKE_STATIC_LIBRARY_SUFFIX})
set(GTEST_LIB_NAME ${CMAKE_STATIC_LIBRARY_PREFIX}gtest${CMAKE_STATIC_LIBRARY_SUFFIX})

target_link_libraries(EngineTest Engine)
target_link_libraries(EngineTest ${GOOGLETEST_BINARY_DIR}${GMOCK_LIB_NAME})
target_link_libraries(EngineTest ${GOOGLETEST_BINARY_DIR}${GTEST_LIB_NAME})

if(MSVC)
    set_property(TARGET EngineTest 
    	APPEND PROPERTY 
    	LINK_FLAGS /DEBUG
    )
endif()

add_custom_command(TARGET EngineTest
	POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory
		"${PROJECT_SOURCE_DIR}/Resource/Test/" 
		"${PROJECT_BINARY_DIR}/bin/Resource/Test/"
)

#set_target_properties(EngineTest
#	PROPERTIES
#	PDB_NAME EngineTest
#)
