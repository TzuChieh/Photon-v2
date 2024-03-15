# Photon-v2 Film Merger Application

get_filename_component(FILM_MERGER_DIR "./" ABSOLUTE)

file(GLOB_RECURSE FILM_MERGER_HEADER_FILES 
	"${FILM_MERGER_DIR}/Source/*.h"
	"${FILM_MERGER_DIR}/Source/*.ipp"
	"${FILM_MERGER_DIR}/Source/*.tpp")
file(GLOB_RECURSE FILM_MERGER_SOURCE_FILES "${FILM_MERGER_DIR}/Source/*.cpp")

# Explicitly mark them as headers
set_source_files_properties(${FILM_MERGER_HEADER_FILES} PROPERTIES HEADER_FILE_ONLY ON)

add_executable(FilmMerger
	${FILM_MERGER_HEADER_FILES}
	${FILM_MERGER_SOURCE_FILES})

add_dependencies(FilmMerger
	Engine)

target_include_directories(FilmMerger
	PRIVATE ${FILM_MERGER_INCLUDE_DIR}
	PRIVATE ${ENGINE_PRIVATE_INCLUDE_DIR})

target_link_libraries(FilmMerger
	PRIVATE Engine)

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    set_property(TARGET FilmMerger 
    	APPEND PROPERTY 
    	LINK_FLAGS /DEBUG:FULL)
endif()

set_target_properties(FilmMerger PROPERTIES 
	VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_BINARY_DIR}")

source_group(TREE ${FILM_MERGER_DIR} FILES ${FILM_MERGER_HEADER_FILES})
source_group(TREE ${FILM_MERGER_DIR} FILES ${FILM_MERGER_SOURCE_FILES})
