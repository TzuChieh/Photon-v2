# Photon-v2 unit tests for engine.

file(GLOB_RECURSE SOURCE_FILES "./Source/*.cpp")
add_executable(EngineTest ${SOURCE_FILES})
add_dependencies(EngineTest Engine)

target_include_directories(EngineTest
	PRIVATE ${GOOGLETEST_INCLUDE_DIR}
	PRIVATE "./Source/"
	PRIVATE ${ENGINE_PRIVATE_INCLUDE_DIR}
	PRIVATE ${LIBSIMDPP_INCLUDE_DIR}
)

target_link_libraries(EngineTest Engine)
target_link_libraries(EngineTest ${GOOGLETEST_LIBS})

if(MSVC)
    set_property(TARGET EngineTest 
    	APPEND PROPERTY 
    	LINK_FLAGS /DEBUG
    )
endif()

#add_custom_command(TARGET EngineTest
#	POST_BUILD
#	COMMAND ${CMAKE_COMMAND} -E copy_directory
#		"${PROJECT_SOURCE_DIR}/Resource/Test/" 
#		"${PROJECT_BINARY_DIR}/bin/Resource/Test/"
#)
