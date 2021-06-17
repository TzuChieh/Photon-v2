#--------------------------------------------------------------------------
# Manually load a library. This is useful for cases where the library is
# found by other means (such as find modules) and is desirable to fit into
# our library loading procedures.
#--------------------------------------------------------------------------
function(load_library_manually libName)

    cmake_policy(SET CMP0074 NEW)

    if(${libName}_LOADED)
        message(VERBOSE 
            "load_library_manually(): ${libName} already loaded, skipping")
        return()
    endif()

    set(${libName}_LOADED FALSE)

    set(OPTIONS
        OPTIONAL)

    set(ONE_VALUE_ARGS
        RUNTIME_DIR)

    set(MULTI_VALUE_ARGS
        TARGETS
        INC_DIRS
        LIB_DIRS
        LIB_NAMES)
    
    cmake_parse_arguments(ARG_MANUAL "${OPTIONS}" "${ONE_VALUE_ARGS}" "${MULTI_VALUE_ARGS}" ${ARGN})

    # Expose cache variables that can be manually set by the user
    set(MANUAL_${libName}_INC_DIRS    "" CACHE PATH 
        "${libName} library's include directories.")
    set(MANUAL_${libName}_LIB_DIRS    "" CACHE PATH 
        "${libName} library's static lib directories.")
    set(MANUAL_${libName}_LIB_NAMES   "" CACHE PATH 
        "${libName} library's static lib names.")
    set(MANUAL_${libName}_RUNTIME_DIR "" CACHE PATH 
        "${libName} library's runtime directory.")

    # Default to empty lists for manual library searching
    if(NOT DEFINED ARG_MANUAL_LIB_NAMES)
        set(ARG_MANUAL_LIB_NAMES "")
    endif()
    if(NOT DEFINED ARG_MANUAL_LIB_DIRS)
        set(ARG_MANUAL_LIB_DIRS "")
    endif()

    set(LIBS_LIST)
    set(ALL_LIBS_FOUND TRUE)
    foreach(LIB_NAME ${ARG_MANUAL_LIB_NAMES})
        string(REPLACE " " "_OR_" LIB_NAME_WITHOUT_SPACES "${LIB_NAME}")
        string(REPLACE " " ";"    LIB_NAME_SEMICOLON_LIST "${LIB_NAME}")

        find_library(${libName}_LIB_${LIB_NAME_WITHOUT_SPACES}
            NAMES ${LIB_NAME_SEMICOLON_LIST}
            PATHS ${ARG_MANUAL_LIB_DIRS}
            NO_DEFAULT_PATH)

        if(${libName}_LIB_${LIB_NAME_WITHOUT_SPACES})
            list(APPEND LIBS_LIST ${${libName}_LIB_${LIB_NAME_WITHOUT_SPACES}})
        else()
            set(ALL_LIBS_FOUND FALSE)
            if(ARG_MANUAL_OPTIONAL)
                message(VERBOSE 
                    "load_library_manually(): ${LIB_NAME_SEMICOLON_LIST} not found")
            else()
                message(SEND_ERROR 
                    "load_library_manually(): ${LIB_NAME_SEMICOLON_LIST} not found, please specify its location via ${libName}_LIB_${LIB_NAME_WITHOUT_SPACES}")
            endif()
        endif()
    endforeach()

    # If targets are specified, library information should be complete
    if(DEFINED ARG_MANUAL_TARGETS)
        set(MANUAL_${libName}_TARGETS ${ARG_MANUAL_TARGETS} PARENT_SCOPE)

        set(${libName}_LOADED TRUE)
    # Otherwise, all libraries must be found and requires at least an
    # include directory specified
    elseif(ALL_LIBS_FOUND AND DEFINED ARG_MANUAL_INC_DIRS)
        # TODO: check include dir and runtime dir exists

        set(MANUAL_${libName}_INCLUDES  ${ARG_MANUAL_INC_DIRS} PARENT_SCOPE)
        set(MANUAL_${libName}_LIBRARIES ${LIBS_LIST}           PARENT_SCOPE)

        set(${libName}_LOADED TRUE)
    endif()

    # Expose general information about how the library is loaded
    if(${libName}_LOADED)
    
        if(NOT DEFINED ARG_MANUAL_RUNTIME_DIR)
            set(ARG_MANUAL_RUNTIME_DIR "")
        endif()

        set(${libName}_LOAD_MODE   "MANUAL"                  PARENT_SCOPE)
        set(${libName}_LOADED      ${${libName}_LOADED}      PARENT_SCOPE)
        set(${libName}_RUNTIME_DIR ${ARG_MANUAL_RUNTIME_DIR} PARENT_SCOPE)

        message(STATUS
            "load_library_manually(): ${libName} loaded")
    else()
        if(ARG_MANUAL_OPTIONAL)
            message(VERBOSE
                "Unable to load library ${libName}")
        else()
            message(SEND_ERROR
                "Unable to load library ${libName}")
        endif()
    endif()
endfunction()
