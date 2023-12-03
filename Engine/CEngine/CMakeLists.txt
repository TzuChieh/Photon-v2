# Photon core engine with C interface.

get_filename_component(CENGINE_DIR "./" ABSOLUTE)

set(CENGINE_INCLUDE_DIR         "${CENGINE_DIR}/Include/")
set(CENGINE_PRIVATE_INCLUDE_DIR "${CENGINE_DIR}/Source/")

file(GLOB_RECURSE CENGINE_HEADER_FILES 
	"${CENGINE_DIR}/Include/*.h" 
	"${CENGINE_DIR}/Source/*.h" 
	"${CENGINE_DIR}/Source/*.ipp" 
	"${CENGINE_DIR}/Source/*.tpp"
)
file(GLOB_RECURSE CENGINE_SOURCE_FILES 
	"${CENGINE_DIR}/Source/*.cpp")

# Explicitly mark them as CEngine headers
set_source_files_properties(${CENGINE_HEADER_FILES} PROPERTIES HEADER_FILE_ONLY ON)

add_library(CEngine ${CENGINE_HEADER_FILES} ${CENGINE_SOURCE_FILES})

target_include_directories(CEngine 
	PUBLIC  ${CENGINE_INCLUDE_DIR}
	PRIVATE ${CENGINE_PRIVATE_INCLUDE_DIR}
	PRIVATE ${ENGINE_PRIVATE_INCLUDE_DIR})

target_link_libraries(CEngine
	PRIVATE Engine)

# Non-Windows platforms do not link to threading and filesystem libraries by default
if(NOT WIN32)
	find_package(Threads)
	target_link_libraries(CEngine PUBLIC ${CMAKE_THREAD_LIBS_INIT})

	# FIXME: remove this when osx supports STL filesystem
	if(NOT APPLE)
		target_link_libraries(CEngine PUBLIC stdc++fs)
	endif()
endif()

source_group(TREE ${CENGINE_DIR} FILES ${CENGINE_HEADER_FILES})
source_group(TREE ${CENGINE_DIR} FILES ${CENGINE_SOURCE_FILES})
