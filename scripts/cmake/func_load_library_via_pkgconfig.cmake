#--------------------------------------------------------------------------
# Try to find library targets by package config files
#--------------------------------------------------------------------------
function(load_library_via_pkgconfig libName)

    # TODO: make config path a multivalueArg
    # TODO: no default path flags
    # TODO: possible targets ignore case? or ignore case as an option
    # TODO: possible to pass a list of directories

    cmake_policy(SET CMP0074 NEW)

    if(${libName}_LOADED)
        message(VERBOSE 
            "load_library_via_pkgconfig(): ${libName} already loaded, skipping")
        return()
    endif()

    set(${libName}_LOADED FALSE)

    set(OPTIONS
        OPTIONAL)

    set(ONE_VALUE_ARGS
        CONFIG_DIR
        RUNTIME_DIR)

    set(MULTI_VALUE_ARGS
        TARGETS)
    
    cmake_parse_arguments(ARG_PKG "${OPTIONS}" "${ONE_VALUE_ARGS}" "${MULTI_VALUE_ARGS}" ${ARGN})

    find_package(PkgConfig QUIET)
    if(NOT PkgConfig_FOUND)
        if(ARG_PKG_OPTIONAL)
            message(VERBOSE 
                "load_library_via_pkgconfig(): pkg-config executable not found, cannot load ${libName}")
        else()
            message(SEND_ERROR 
                "load_library_via_pkgconfig(): pkg-config executable not found, cannot load ${libName}")
        endif()
        return()
    endif()

    # Now we are sure we have the pkg-config executable located

    # Hint pkg-config to find package config files from additional directories
    set(CMAKE_PREFIX_PATH    "${ARG_PKG_CONFIG_DIR}")
    set(ENV{PKG_CONFIG_PATH} "${ARG_PKG_CONFIG_DIR}")

    set(PKG_${libName}_TARGETS)
    set(ALL_TARGETS_FOUND TRUE)
    foreach(TARGET_NAME ${ARG_PKG_TARGETS})
        # Look for .pc file and creates an imported target 
        # named PkgConfig::${TARGET_NAME}_PKG (IMPORTED_TARGET requires CMake >= 3.6.3)
        pkg_search_module(${TARGET_NAME}_PKG QUIET IMPORTED_TARGET ${TARGET_NAME})

        if(${TARGET_NAME}_PKG_FOUND)
            message(VERBOSE 
                "load_library_via_pkgconfig(): Found package ${TARGET_NAME} for ${libName}")

            list(APPEND PKG_${libName}_TARGETS "PkgConfig::${TARGET_NAME}_PKG")

            # Promote the pkg config target to global space in order to name an alias for it
            set_target_properties("PkgConfig::${TARGET_NAME}_PKG" 
                PROPERTIES IMPORTED_GLOBAL TRUE)

            # Some other libraries may depend on conventionally-named target
            add_library("${libName}::${TARGET_NAME}" ALIAS "PkgConfig::${TARGET_NAME}_PKG")
        else()
            set(ALL_TARGETS_FOUND FALSE)

            message(VERBOSE 
                "load_library_via_pkgconfig(): Failed to load target ${libName}")
        endif()
    endforeach()

    unset(CMAKE_PREFIX_PATH)
    unset(ENV{PKG_CONFIG_PATH})

    set(PKG_${libName}_TARGETS ${PKG_${libName}_TARGETS} PARENT_SCOPE)

    # TODO: consider using "sharedlibdir" entry (first check for existence in .pc file) for runtime dir

    # Expose general information about how the library is loaded
    if(ALL_TARGETS_FOUND)
        set(${libName}_LOADED      TRUE                   PARENT_SCOPE)
        set(${libName}_LOAD_MODE  "PKG"                   PARENT_SCOPE)
        set(${libName}_RUNTIME_DIR ${ARG_PKG_RUNTIME_DIR} PARENT_SCOPE)

        message(STATUS
            "load_library_via_pkgconfig(): ${libName} loaded")
    elseif(NOT ARG_PKG_OPTIONAL)
        message(SEND_ERROR 
            "load_library_via_pkgconfig(): Not all targets for ${libName} are found")
    endif()
endfunction()
