#--------------------------------------------------------------------------
# Try to find library targets by project config files
#--------------------------------------------------------------------------
function(load_library_via_config libName)

    # TODO: make config path a multivalueArg
    # TODO: no default path flags
    # TODO: possible targets ignore case? or ignore case as an option
    # TODO: possible to pass a list of directories

    cmake_policy(SET CMP0074 NEW)

    if(${libName}_LOADED)
        message(VERBOSE 
            "load_library_via_config(): ${libName} already loaded, skipping")
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
    
    cmake_parse_arguments(ARG_CFG "${OPTIONS}" "${ONE_VALUE_ARGS}" "${MULTI_VALUE_ARGS}" ${ARGN})
    
    # Try to find library targets with find_package() in config mode 
    # (without CMake default paths)
    find_package(${libName} CONFIG QUIET
        PATHS "${ARG_CFG_CONFIG_DIR}/"
        NO_DEFAULT_PATH)

    # Find again with CMake default paths (if previous attempt succeeded, the
    # call will be automatically skipped due to ${libName}_FOUND being set)
    find_package(${libName} CONFIG QUIET)

    if(${libName}_FOUND)
        message(STATUS 
            "load_library_via_config(): Found ${libName}")

        set(TARGETS_LIST)
        foreach(TARGET_NAME ${ARG_CFG_TARGETS})
            list(APPEND TARGETS_LIST "${libName}::${TARGET_NAME}")
        endforeach()
        set(CONFIG_${libName}_TARGETS ${TARGETS_LIST} PARENT_SCOPE)
        
        # TODO: check target exists

        set(${libName}_LOADED TRUE)
    else()
        message(VERBOSE 
            "load_library_via_config(): Config file of ${libName} not found")
    endif()

    # Expose general information about how the library is loaded
    if(${libName}_LOADED)
        set(${libName}_LOAD_MODE   "CONFIG"               PARENT_SCOPE)
        set(${libName}_LOADED      ${${libName}_LOADED}   PARENT_SCOPE)
        set(${libName}_RUNTIME_DIR ${ARG_CFG_RUNTIME_DIR} PARENT_SCOPE)

        message(STATUS
            "load_library_via_config(): ${libName} loaded")
    elseif(NOT ARG_CFG_OPTIONAL)
        message(SEND_ERROR
            "load_library_via_config(): Unable to load ${libName}")
    endif()
endfunction()
