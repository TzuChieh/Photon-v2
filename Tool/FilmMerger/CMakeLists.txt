# Photon-v2 Film Merger Application

get_filename_component(FILM_MERGER_DIR "./" ABSOLUTE)

set(TARGET_NAME "FilmMerger")

file(GLOB_RECURSE FILM_MERGER_HEADER_FILES 
	"${FILM_MERGER_DIR}/Source/*.h"
	"${FILM_MERGER_DIR}/Source/*.ipp"
	"${FILM_MERGER_DIR}/Source/*.tpp")
file(GLOB_RECURSE FILM_MERGER_SOURCE_FILES "${FILM_MERGER_DIR}/Source/*.cpp")

# Explicitly mark them as headers
set_source_files_properties(${FILM_MERGER_HEADER_FILES} PROPERTIES HEADER_FILE_ONLY ON)

add_executable(${TARGET_NAME}
	${FILM_MERGER_HEADER_FILES}
	${FILM_MERGER_SOURCE_FILES})

add_dependencies(${TARGET_NAME}
	Engine)

target_include_directories(${TARGET_NAME}
	PRIVATE "${FILM_MERGER_DIR}/Source"
	PRIVATE ${ENGINE_PRIVATE_INCLUDE_DIR})

target_link_libraries(${TARGET_NAME}
	PRIVATE Engine)

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    set_property(TARGET ${TARGET_NAME} 
    	APPEND PROPERTY 
    	LINK_FLAGS /DEBUG:FULL)
endif()

set_target_properties(${TARGET_NAME} PROPERTIES 
	VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_BINARY_DIR}")

source_group(TREE ${FILM_MERGER_DIR} FILES ${FILM_MERGER_HEADER_FILES})
source_group(TREE ${FILM_MERGER_DIR} FILES ${FILM_MERGER_SOURCE_FILES})
