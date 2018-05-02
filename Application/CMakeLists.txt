
# Photon-v2 application.

file(GLOB_RECURSE PHOTON_CLI_HEADER_FILES 
	"./*.h"
	"./*.ipp"
	"./*.tpp"
)
file(GLOB_RECURSE PHOTON_CLI_SOURCE_FILES "./*.cpp")

# explicitly mark them as headers
#
set_source_files_properties(${PHOTON_CLI_HEADER_FILES} PROPERTIES HEADER_FILE_ONLY TRUE)

add_executable(Application ${PHOTON_CLI_HEADER_FILES} ${PHOTON_CLI_SOURCE_FILES})

# setup dependencies

add_dependencies(Application Engine)

target_include_directories(Application
	PRIVATE ${ENGINE_INCLUDE_DIR}
)

target_link_libraries(Application Engine)
