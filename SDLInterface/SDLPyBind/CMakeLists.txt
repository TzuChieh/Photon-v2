# Photon scene description language Python binding library.

get_filename_component(SDL_PY_BIND_DIR "./" ABSOLUTE)

set(TARGET_NAME                     "SDLPyBind")
set(SDL_PY_BIND_INCLUDE_DIR         "${SDL_PY_BIND_DIR}/Include")
set(SDL_PY_BIND_PRIVATE_INCLUDE_DIR "${SDL_PY_BIND_DIR}/Source")

file(GLOB_RECURSE SDL_GEN_HEADER_FILES 
	"${SDL_PY_BIND_DIR}/Include/*.h"
	"${SDL_PY_BIND_DIR}/Source/*.h"
	"${SDL_PY_BIND_DIR}/Source/*.ipp"
	"${SDL_PY_BIND_DIR}/Source/*.tpp"
)
file(GLOB_RECURSE SDL_PY_BIND_SOURCE_FILES "${SDL_PY_BIND_DIR}/Source/*.cpp")

# Explicitly mark them as headers
set_source_files_properties(${SDL_PY_BIND_HEADER_FILES} PROPERTIES HEADER_FILE_ONLY ON)

# Module library as it will be loaded dynamically
add_library(${TARGET_NAME} MODULE ${SDL_PY_BIND_HEADER_FILES} ${SDL_PY_BIND_SOURCE_FILES})

add_dependencies(${TARGET_NAME} Engine)

target_include_directories(${TARGET_NAME}
	PUBLIC  ${SDL_PY_BIND_INCLUDE_DIR}
	PRIVATE ${SDL_PY_BIND_PRIVATE_INCLUDE_DIR}
	PRIVATE ${ENGINE_PRIVATE_INCLUDE_DIR})

target_link_libraries(${TARGET_NAME} PUBLIC Engine)

# nanobind CMake doc: https://nanobind.readthedocs.io/en/latest/api_cmake.html#lowlevel-cmake

# Build the core parts of nanobind once (-abi3 as we want to target stable Python ABI)
set(NANOBIND_LIB_NAME "nanobind-abi3")
nanobind_build_library(${NANOBIND_LIB_NAME} SHARED)

target_link_libraries(${TARGET_NAME} PRIVATE ${NANOBIND_LIB_NAME})

set(PH_PY_BIND_MODULE_NAME "ph")
target_compile_definitions(${TARGET_NAME} PRIVATE PH_PY_BIND_MODULE_NAME=${PH_PY_BIND_MODULE_NAME})

# The actual name of the binary must match module name
set_target_properties(${TARGET_NAME} PROPERTIES OUTPUT_NAME ${PH_PY_BIND_MODULE_NAME})

# .. enable size optimizations
nanobind_opt_size(${TARGET_NAME})

# .. enable link time optimization
nanobind_lto(${TARGET_NAME})

# .. set the default symbol visibility to 'hidden'
nanobind_set_visibility(${TARGET_NAME})

# .. strip unneeded symbols and debug info from the binary (only active in release builds)
nanobind_strip(${TARGET_NAME})

# .. disable the stack protector
nanobind_disable_stack_protector(${TARGET_NAME})

# .. set the Python extension suffix
nanobind_extension(${TARGET_NAME})

# .. set important compilation flags
nanobind_compile_options(${TARGET_NAME})

# .. set important linker flags
nanobind_link_options(${TARGET_NAME})

# Statically link against libstdc++/libgcc when targeting musllinux
nanobind_musl_static_libcpp(${TARGET_NAME})

# Put new targets in dedicated filter in Visual Studio
set_target_properties(${TARGET_NAME} ${NANOBIND_LIB_NAME}
	PROPERTIES FOLDER "PythonBindings")

source_group(TREE ${SDL_PY_BIND_DIR} FILES ${SDL_PY_BIND_HEADER_FILES})
source_group(TREE ${SDL_PY_BIND_DIR} FILES ${SDL_PY_BIND_SOURCE_FILES})
