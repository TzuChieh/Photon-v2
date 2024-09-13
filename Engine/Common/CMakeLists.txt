# Photon Common library.

get_filename_component(COMMON_LIB_DIR "./" ABSOLUTE)

set(COMMON_LIB_INCLUDE_DIR         "${COMMON_LIB_DIR}/Include/")
set(COMMON_LIB_PRIVATE_INCLUDE_DIR "${COMMON_LIB_DIR}/Source/")

file(GLOB_RECURSE COMMON_LIB_HEADER_FILES 
	"${COMMON_LIB_DIR}/Include/*.h"
	"${COMMON_LIB_DIR}/Source/*.h" 
	"${COMMON_LIB_DIR}/Source/*.ipp" 
	"${COMMON_LIB_DIR}/Source/*.tpp"
)
file(GLOB_RECURSE COMMON_LIB_SOURCE_FILES 
	"${COMMON_LIB_DIR}/Source/*.cpp")

# Explicitly mark them as headers
set_source_files_properties(${COMMON_LIB_HEADER_FILES} PROPERTIES HEADER_FILE_ONLY ON)

add_library(Common ${COMMON_LIB_HEADER_FILES} ${COMMON_LIB_SOURCE_FILES})

target_include_directories(Common 
	PUBLIC  ${COMMON_LIB_INCLUDE_DIR}
	PRIVATE ${COMMON_LIB_PRIVATE_INCLUDE_DIR})

# Non-Windows platforms do not link to threading and filesystem libraries by default
if(NOT WIN32)
	find_package(Threads)
	target_link_libraries(Common PUBLIC ${CMAKE_THREAD_LIBS_INIT})

	# FIXME: remove this when osx supports STL filesystem
	if(NOT APPLE)
		target_link_libraries(Common PUBLIC stdc++fs)
	endif()
endif()

include("${CMAKE_SOURCE_DIR}/scripts/cmake/func_link_thirdparty_lib.cmake")

link_thirdparty_lib(Common ${TRACY_LIB_NAME} PUBLIC OPTIONAL
	DETECTING_PREPROCESSOR PH_THIRD_PARTY_HAS_TRACY)

# Using MSVC
if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
	target_compile_definitions(Common
		PUBLIC _CRT_SECURE_NO_WARNINGS)
# Using gcc/g++
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	# We need `libstdc++exp.a` for `std::stacktrace` for now (g++ 14.0.1)
	# https://gcc.gnu.org/gcc-14/changes.html
	target_link_libraries(Common
		PRIVATE stdc++exp)
endif()

if(PH_ENABLE_DEBUG)
	target_compile_definitions(Common
		PUBLIC PH_CONFIG_ENABLE_DEBUG)
endif()

if(PH_ENABLE_PROFILING)
	target_compile_definitions(Common
		PUBLIC PH_CONFIG_ENABLE_PROFILING)
endif()

if(PH_DOUBLE_PRECISION_REAL)
	target_compile_definitions(Common
		PUBLIC PH_CONFIG_DOUBLE_PRECISION_REAL)
endif()

if(PH_USE_SIMD)
	target_compile_definitions(Common
		PUBLIC PH_CONFIG_USE_SIMD)

	if(PH_HARDWARE_HAS_SSE)
		target_compile_definitions(Common
			PUBLIC PH_CONFIG_HARDWARE_HAS_SSE)
	endif()
	if(PH_HARDWARE_HAS_SSE2)
		target_compile_definitions(Common
			PUBLIC PH_CONFIG_HARDWARE_HAS_SSE2)
	endif()
	if(PH_HARDWARE_HAS_SSE3)
		target_compile_definitions(Common
			PUBLIC PH_CONFIG_HARDWARE_HAS_SSE3)
	endif()
	if(PH_HARDWARE_HAS_SSSE3)
		target_compile_definitions(Common
			PUBLIC PH_CONFIG_HARDWARE_HAS_SSSE3)
	endif()
	if(PH_HARDWARE_HAS_SSE4_1)
		target_compile_definitions(Common
			PUBLIC PH_CONFIG_HARDWARE_HAS_SSE4_1)
	endif()
	if(PH_HARDWARE_HAS_SSE4_2)
		target_compile_definitions(Common
			PUBLIC PH_CONFIG_HARDWARE_HAS_SSE4_2)
	endif()
	if(PH_HARDWARE_HAS_AVX)
		target_compile_definitions(Common
			PUBLIC PH_CONFIG_HARDWARE_HAS_AVX)
	endif()
	if(PH_HARDWARE_HAS_AVX2)
		target_compile_definitions(Common
			PUBLIC PH_CONFIG_HARDWARE_HAS_AVX2)
	endif()
endif()

source_group(TREE ${COMMON_LIB_DIR} FILES ${COMMON_LIB_HEADER_FILES})
source_group(TREE ${COMMON_LIB_DIR} FILES ${COMMON_LIB_SOURCE_FILES})
