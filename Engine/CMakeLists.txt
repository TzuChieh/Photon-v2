# Photon-v2 core engine.

get_filename_component(ENGINE_DIR "./" ABSOLUTE)

set(ENGINE_INCLUDE_DIR         "${ENGINE_DIR}/Include/")
set(ENGINE_PRIVATE_INCLUDE_DIR "${ENGINE_DIR}/Source/")

file(GLOB_RECURSE ENGINE_HEADER_FILES 
	"./Include/*.h" 
	"./Source/*.h" 
	"./Source/*.ipp" 
	"./Source/*.tpp"
)
file(GLOB_RECURSE ENGINE_SOURCE_FILES "./Source/*.cpp")

# explicitly mark them as headers
#
set_source_files_properties(${ENGINE_HEADER_FILES} PROPERTIES HEADER_FILE_ONLY TRUE)

add_library(Engine STATIC ${ENGINE_HEADER_FILES} ${ENGINE_SOURCE_FILES})

# obj-lib seems to have pdb files, but libraries generated from it
# do not. this can cause numerous warnings in googletest

#[[

# compiling engine source only once to get an object library
#
add_library(EngineObjLib OBJECT ${SOURCE_FILES})

# shared libraries need PIC (static libraries do not care about this)
#
set_property(TARGET EngineObjLib PROPERTY POSITION_INDEPENDENT_CODE 1)

]]

target_include_directories(Engine 
	PRIVATE ${ENGINE_INCLUDE_DIR}
	PRIVATE ${ENGINE_PRIVATE_INCLUDE_DIR}
	PRIVATE ${STB_INCLUDE_DIR}
)

# other platforms do not link to threading and filesystem libraries by default
if(NOT WIN32)

	find_package(Threads)
	target_link_libraries(Engine 
		${CMAKE_THREAD_LIBS_INIT}
	)

	# FIXME: remove this when osx supports STL filesystem
	if(NOT APPLE)
		target_link_libraries(Engine stdc++fs)
	endif()

endif()

target_compile_definitions(Engine
	PRIVATE _CRT_SECURE_NO_WARNINGS
)

#[[

# shared and static libraries built from the same object files
#
add_library(Engine       SHARED $<TARGET_OBJECTS:EngineObjLib>)
add_library(EngineStatic STATIC $<TARGET_OBJECTS:EngineObjLib>)

]]

# make these directories accessible for other sub-projects
#
set(ENGINE_INCLUDE_DIR         "${ENGINE_INCLUDE_DIR}"         PARENT_SCOPE)
set(ENGINE_PRIVATE_INCLUDE_DIR "${ENGINE_PRIVATE_INCLUDE_DIR}" PARENT_SCOPE)
