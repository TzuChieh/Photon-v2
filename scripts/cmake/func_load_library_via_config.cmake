#--------------------------------------------------------------------------
# Try to find library targets by project config files
#
# The `CONFIG_NAMES` option can be supplied to override the name used for config file searching
# (each supplied name will be tried for searching config file; default behavior is to use
# `libName` only).
#
# A CMake convention is to use double colon to prefix target name with package name,
# e.g., myPkg::myTarget. However, not all libraries define their target names this way.
# The `NO_PREFIX_ON_TARGETS` option can disable the behavior that uses `libName` as target
# name prefix.
#
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
        OPTIONAL
        NO_PREFIX_ON_TARGETS)

    set(ONE_VALUE_ARGS
        CONFIG_DIR
        RUNTIME_DIR)

    set(MULTI_VALUE_ARGS
        TARGETS
        CONFIG_NAMES)
    
    cmake_parse_arguments(ARG_CFG "${OPTIONS}" "${ONE_VALUE_ARGS}" "${MULTI_VALUE_ARGS}" ${ARGN})

    if(ARG_CFG_OPTIONAL)
    	set(REQUIREMENT QUIET)
    else()
        set(REQUIREMENT)
    endif()

    # Use library name as config name by default
    if(NOT DEFINED ARG_CFG_CONFIG_NAMES)
        set(ARG_CFG_CONFIG_NAMES ${libName})
    endif()
    
    # find_package() command will search for a file called "<name>Config.cmake" 
    # or "<lower-case-name>-config.cmake" for each name specified (via the 
    # NAMES option)

    # Try to find library targets with find_package() in config mode 
    # (without CMake default paths)
    find_package(${libName} CONFIG ${REQUIREMENT}
        PATHS "${ARG_CFG_CONFIG_DIR}/"
        NO_DEFAULT_PATH
        NAMES ${ARG_CFG_CONFIG_NAMES})

    if(NOT ${libName}_FOUND)
        message(WARNING 
            "load_library_via_config(): ${libName} not found on specified config path, retrying with default path")
    endif()

    # Find again with CMake default paths (if previous attempt succeeded, the
    # call will be automatically skipped due to ${libName}_FOUND being set)
    find_package(${libName} CONFIG QUIET
        NAMES ${ARG_CFG_CONFIG_NAMES})

    if(${libName}_FOUND)
        message(STATUS 
            "load_library_via_config(): Found ${libName} (considered configs: ${${libName}_CONSIDERED_CONFIGS})")

        set(TARGETS_LIST)
        foreach(TARGET_NAME ${ARG_CFG_TARGETS})
            if(ARG_CFG_NO_PREFIX_ON_TARGETS)
                list(APPEND TARGETS_LIST "${TARGET_NAME}")
            else()
                list(APPEND TARGETS_LIST "${libName}::${TARGET_NAME}")
            endif()

            # TODO: currently cannot properly detect interface library
            # Check if target exists and add/warn for it
            # if (TARGET ${TARGET_NAME})
            #     list(APPEND TARGETS_LIST "${libName}::${TARGET_NAME}")
            # else()
            #     if(NOT ARG_CFG_OPTIONAL)
            #         message(WARNING 
            #             "load_library_via_config(): target ${TARGET} of ${libName} not found")
            #     endif()
            # endif()
        endforeach()

        set(CONFIG_${libName}_TARGETS ${TARGETS_LIST} PARENT_SCOPE)
        set(${libName}_LOADED TRUE)
    else()
        if(NOT ARG_CFG_OPTIONAL)
            message(STATUS 
                "load_library_via_config(): Config file of ${libName} not found")
        endif()
    endif()

    # Expose general information about how the library is loaded
    if(${libName}_LOADED)

        if(NOT DEFINED ARG_CFG_RUNTIME_DIR)
            set(ARG_CFG_RUNTIME_DIR "")
        endif()

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
