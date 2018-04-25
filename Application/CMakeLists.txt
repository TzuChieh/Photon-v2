
# Photon-v2 application.

file(GLOB_RECURSE SOURCE_FILES "./*.cpp")
add_executable(Application ${SOURCE_FILES})

add_dependencies(Application Engine)

target_include_directories(Application
	PRIVATE ${ENGINE_INCLUDE_DIR}
)

target_link_libraries(Application Engine)
