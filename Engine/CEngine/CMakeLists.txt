# Photon core engine with C interface.

get_filename_component(CENGINE_DIR "./" ABSOLUTE)

set(TARGET_NAME                 "CEngine")
set(CENGINE_INCLUDE_DIR         "${CENGINE_DIR}/Include")
set(CENGINE_PRIVATE_INCLUDE_DIR "${CENGINE_DIR}/Source")

file(GLOB_RECURSE CENGINE_HEADER_FILES 
	"${CENGINE_INCLUDE_DIR}/${TARGET_NAME}/*.h" 
	"${CENGINE_PRIVATE_INCLUDE_DIR}/${TARGET_NAME}/*.h" 
	"${CENGINE_PRIVATE_INCLUDE_DIR}/${TARGET_NAME}/*.ipp" 
	"${CENGINE_PRIVATE_INCLUDE_DIR}/${TARGET_NAME}/*.tpp"
)
file(GLOB_RECURSE CENGINE_SOURCE_FILES 
	"${CENGINE_PRIVATE_INCLUDE_DIR}/${TARGET_NAME}/*.cpp")

# Explicitly mark them as headers
set_source_files_properties(${CENGINE_HEADER_FILES} PROPERTIES HEADER_FILE_ONLY ON)

add_library(${TARGET_NAME} ${CENGINE_HEADER_FILES} ${CENGINE_SOURCE_FILES})

target_include_directories(${TARGET_NAME} 
	PUBLIC  ${CENGINE_INCLUDE_DIR}
	PRIVATE ${CENGINE_PRIVATE_INCLUDE_DIR}
	PRIVATE ${ENGINE_PRIVATE_INCLUDE_DIR})

target_link_libraries(${TARGET_NAME}
	PRIVATE Engine)

# Non-Windows platforms do not link to threading and filesystem libraries by default
if(NOT WIN32)
	find_package(Threads)
	target_link_libraries(${TARGET_NAME} PUBLIC ${CMAKE_THREAD_LIBS_INIT})

	# FIXME: remove this when osx supports STL filesystem
	if(NOT APPLE)
		target_link_libraries(${TARGET_NAME} PUBLIC stdc++fs)
	endif()
endif()

source_group(TREE ${CENGINE_DIR} FILES ${CENGINE_HEADER_FILES})
source_group(TREE ${CENGINE_DIR} FILES ${CENGINE_SOURCE_FILES})
