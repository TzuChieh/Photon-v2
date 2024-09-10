# Photon core engine.

get_filename_component(ENGINE_DIR "./" ABSOLUTE)

set(ENGINE_INCLUDE_DIR         "${ENGINE_DIR}/Include/")
set(ENGINE_PRIVATE_INCLUDE_DIR "${ENGINE_DIR}/Source/")

file(GLOB_RECURSE ENGINE_HEADER_FILES 
	"${ENGINE_DIR}/Include/*.h" 
	"${ENGINE_DIR}/Source/*.h" 
	"${ENGINE_DIR}/Source/*.ipp" 
	"${ENGINE_DIR}/Source/*.tpp"
)
file(GLOB_RECURSE ENGINE_SOURCE_FILES 
	"${ENGINE_DIR}/Source/*.cpp")

# Explicitly mark them as engine headers
set_source_files_properties(${ENGINE_HEADER_FILES} PROPERTIES HEADER_FILE_ONLY ON)

add_library(Engine ${ENGINE_HEADER_FILES} ${ENGINE_SOURCE_FILES})

# obj-lib seems to have pdb files, but libraries generated from it
# do not. this can cause numerous warnings in googletest

#[[

# compiling engine source only once to get an object library
add_library(EngineObjLib OBJECT ${SOURCE_FILES})

# shared libraries need PIC (static libraries do not care about this)
set_property(TARGET EngineObjLib PROPERTY POSITION_INDEPENDENT_CODE 1)

]]

target_include_directories(Engine 
	PUBLIC  ${ENGINE_INCLUDE_DIR}
	PRIVATE ${ENGINE_PRIVATE_INCLUDE_DIR})

target_link_libraries(Engine
	PUBLIC Common)

include("${CMAKE_SOURCE_DIR}/scripts/cmake/func_link_thirdparty_lib.cmake")

link_thirdparty_lib(Engine ${STB_LIB_NAME})
link_thirdparty_lib(Engine ${IMATH_LIB_NAME} OPTIONAL)
link_thirdparty_lib(Engine ${OPENEXR_LIB_NAME} OPTIONAL
	DETECTING_PREPROCESSOR PH_THIRD_PARTY_HAS_OPENEXR)
link_thirdparty_lib(Engine ${CONCURRENTQUEUE_LIB_NAME} PUBLIC)

#[[

# shared and static libraries built from the same object files
add_library(Engine       SHARED $<TARGET_OBJECTS:EngineObjLib>)
add_library(EngineStatic STATIC $<TARGET_OBJECTS:EngineObjLib>)

]]

source_group(TREE ${ENGINE_DIR} FILES ${ENGINE_HEADER_FILES})
source_group(TREE ${ENGINE_DIR} FILES ${ENGINE_SOURCE_FILES})

# TODO: other files such as .dox

# Make these directories accessible for other sub-projects
set(ENGINE_PRIVATE_INCLUDE_DIR "${ENGINE_PRIVATE_INCLUDE_DIR}" PARENT_SCOPE)
