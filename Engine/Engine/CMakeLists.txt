# Photon-v2 core engine.

get_filename_component(ENGINE_DIR "./" ABSOLUTE)

set(ENGINE_C_INCLUDE_DIR        "${ENGINE_DIR}/Include/CEngine/")
set(ENGINE_CPP_INCLUDE_DIR      "${ENGINE_DIR}/Include/CppEngine/")
set(ENGINE_CPP_IMPL_INCLUDE_DIR "${ENGINE_DIR}/Source/")

set(ENGINE_C_API_INCLUDE_DIRS
	"${ENGINE_C_INCLUDE_DIR}")

set(ENGINE_CPP_API_INCLUDE_DIRS
	"${ENGINE_CPP_INCLUDE_DIR}"
	"${ENGINE_CPP_IMPL_INCLUDE_DIR}")

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
	PUBLIC  ${ENGINE_C_API_INCLUDE_DIRS}
	PRIVATE ${ENGINE_CPP_API_INCLUDE_DIRS})

include("${CMAKE_SOURCE_DIR}/scripts/cmake/func_link_thirdparty_lib.cmake")

link_thirdparty_lib(Engine ${STB_LIB_NAME})
link_thirdparty_lib(Engine ${ZLIB_LIB_NAME})
link_thirdparty_lib(Engine ${ILMBASE_LIB_NAME})
link_thirdparty_lib(Engine ${OPENEXR_LIB_NAME})
link_thirdparty_lib(Engine ${CONCURRENTQUEUE_LIB_NAME} PUBLIC)
link_thirdparty_lib(Engine ${TRACY_LIB_NAME} PUBLIC OPTIONAL
	DETECTING_PREPROCESSOR PH_THIRD_PARTY_HAS_TRACY)

# Non-Windows platforms do not link to threading and filesystem libraries by default
if(NOT WIN32)
	find_package(Threads)
	target_link_libraries(Engine PUBLIC ${CMAKE_THREAD_LIBS_INIT})

	# FIXME: remove this when osx supports STL filesystem
	if(NOT APPLE)
		target_link_libraries(Engine PUBLIC stdc++fs)
	endif()
endif()

# Using MSVC
if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
	target_compile_definitions(Engine
		PUBLIC _CRT_SECURE_NO_WARNINGS)
endif()

if(PH_ENABLE_DEBUG)
	target_compile_definitions(Engine
		PUBLIC PH_CONFIG_ENABLE_DEBUG)
endif()

if(PH_ENABLE_PROFILING)
	target_compile_definitions(Engine
		PUBLIC PH_CONFIG_ENABLE_PROFILING)
endif()

if(PH_DOUBLE_PRECISION_REAL)
	target_compile_definitions(Engine
		PUBLIC PH_CONFIG_DOUBLE_PRECISION_REAL)
endif()

#[[

# shared and static libraries built from the same object files
add_library(Engine       SHARED $<TARGET_OBJECTS:EngineObjLib>)
add_library(EngineStatic STATIC $<TARGET_OBJECTS:EngineObjLib>)

]]

source_group(TREE ${ENGINE_DIR} FILES ${ENGINE_HEADER_FILES})
source_group(TREE ${ENGINE_DIR} FILES ${ENGINE_SOURCE_FILES})

# TODO: other files such as .dox

# Make these directories accessible for other sub-projects
set(ENGINE_CPP_API_INCLUDE_DIRS "${ENGINE_CPP_API_INCLUDE_DIRS}" PARENT_SCOPE)
