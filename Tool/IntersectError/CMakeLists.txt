# Photon-v2 Intersect Error Application

get_filename_component(INTERSECT_ERROR_DIR "./" ABSOLUTE)

file(GLOB_RECURSE INTERSECT_ERROR_HEADER_FILES 
	"${INTERSECT_ERROR_DIR}/Source/*.h"
	"${INTERSECT_ERROR_DIR}/Source/*.ipp"
	"${INTERSECT_ERROR_DIR}/Source/*.tpp")
file(GLOB_RECURSE INTERSECT_ERROR_SOURCE_FILES "${INTERSECT_ERROR_DIR}/Source/*.cpp")

# Explicitly mark them as headers
set_source_files_properties(${INTERSECT_ERROR_HEADER_FILES} PROPERTIES HEADER_FILE_ONLY ON)

add_executable(IntersectError
	${INTERSECT_ERROR_HEADER_FILES}
	${INTERSECT_ERROR_SOURCE_FILES})

add_dependencies(IntersectError
	Engine)

target_include_directories(IntersectError
	PRIVATE ${INTERSECT_ERROR_INCLUDE_DIR})

target_link_libraries(IntersectError
	PRIVATE Engine)

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    set_property(TARGET IntersectError 
    	APPEND PROPERTY 
    	LINK_FLAGS /DEBUG:FULL)
endif()

set_target_properties(IntersectError PROPERTIES 
	VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_BINARY_DIR}")

source_group(TREE ${INTERSECT_ERROR_DIR} FILES ${INTERSECT_ERROR_HEADER_FILES})
source_group(TREE ${INTERSECT_ERROR_DIR} FILES ${INTERSECT_ERROR_SOURCE_FILES})
