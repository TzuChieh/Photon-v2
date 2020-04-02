# TODO: make config path a multivalueArg
function(load_thirdparty_lib libName)
    if(${libName}_LOADED)
        message(VERBOSE 
            "Library ${libName} already loaded, skipping...")
        return()
    endif()

    set(${libName}_LOADED FALSE)

    set(options 
        OPTIONAL)
    set(oneValueArgs 
        PACKAGE_PATH
        CONFIG_PATH)
    set(multiValueArgs 
        CONFIG_TARGETS
        MANUAL_INC_DIRS 
        MANUAL_LIB_DIRS 
        MANUAL_LIB_NAMES)
    cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    # TODO: possible vars ignore case?
    # TODO: add module mode

    set(POSSIBLE_INCLUDE_DIR_VARS
        "${libName}_INCLUDE_DIRS"
        "${libName}_INCLUDES"
        "${libName}_INCLUDE_DIR"
        "${libName}_INCLUDE")

    set(POSSIBLE_LIB_VARS
        "${libName}_LIBRARIES"
        "${libName}_LIBS"
        "${libName}_LIBRARY"
        "${libName}_LIB")

    # Try to unify include directories and libraries variables (if they exists)

    foreach(INCLUDE_DIR_VAR ${POSSIBLE_INCLUDE_DIR_VARS})
        if(${INCLUDE_DIR_VAR})
            set(${libName}_INCLUDE_DIRS "${${INCLUDE_DIR_VAR}}")
            break()
        endif()
    endforeach()

    foreach(LIB_VAR IN ITEMS ${POSSIBLE_LIB_VARS})
        if(${LIB_VAR})
            set(${libName}_LIBRARIES "${${LIB_VAR}}")
            break()
        endif()
    endforeach()

    #--------------------------------------------------------------------------
    # First check if existing library variables are properly defined
    #--------------------------------------------------------------------------
    if(NOT ${libName}_LOADED AND ${libName}_INCLUDE_DIRS AND ${libName}_LIBRARIES)
        message(STATUS 
            "Found existing ${libName} library variables.")

        set(VARS_${libName}_INCLUDE_DIRS "${${libName}_INCLUDE_DIRS}" PARENT_SCOPE)
        set(VARS_${libName}_LIBRARIES    "${${libName}_LIBRARIES}"    PARENT_SCOPE)

        set(${libName}_LOAD_MODE "VARS")
        set(${libName}_LOADED     TRUE)
    endif()

    #--------------------------------------------------------------------------
    # Try to find library targets by package config files
    #--------------------------------------------------------------------------
    if(NOT ${libName}_LOADED AND ARG_PACKAGE_PATH)
        # TODO: possible to pass a list of directories
        set(CMAKE_PREFIX_PATH "${ARG_PACKAGE_PATH}")
        set(ENV{PKG_CONFIG_PATH}  "${ARG_PACKAGE_PATH}")
        message(STATUS 
        "${CMAKE_PREFIX_PATH}")
        find_package(PkgConfig QUIET)
        if(PkgConfig_FOUND)
            # Look for .pc file and creates an imported target named PkgConfig::${libName}_PKG
            # (IMPORTED_TARGET requires CMake >= 3.6.3)
            pkg_search_module(${libName}_PKG REQUIRED IMPORTED_TARGET gmock)

            if(${libName}_PKG_FOUND)
                message(STATUS 
                    "Found ${libName} in package mode.")

                set(PKG_${libName}_TARGETS "PkgConfig::${libName}_PKG" PARENT_SCOPE)

                set(${libName}_LOAD_MODE "PKG")
                set(${libName}_LOADED     TRUE)
            else()
                message(VERBOSE 
                    "Package config file for ${libName} not found.")
            endif()
        else()
            message(VERBOSE 
                "PkgConfig not found, cannot load ${libName} via package mode.")
        endif()
    endif()

    #--------------------------------------------------------------------------
    # Try to find library targets by project config files
    #--------------------------------------------------------------------------
    if(NOT ${libName}_LOADED AND ARG_CONFIG_PATH)
        # Try to find library targets with find_package() config mode
        find_package(${libName} CONFIG QUIET
            PATHS "${ARG_CONFIG_PATH}/"
            NO_DEFAULT_PATH)

        if(${libName}_FOUND)
            message(STATUS 
                "Found ${libName} in config mode.")

            set(TARGETS_LIST)
            foreach(TARGET_NAME ${ARG_CONFIG_TARGETS})
                list(APPEND TARGETS_LIST "${libName}::${TARGET_NAME}")
            endforeach()
            set(CONFIG_${libName}_TARGETS ${TARGETS_LIST} PARENT_SCOPE)

            set(${libName}_LOAD_MODE "CONFIG")
            set(${libName}_LOADED    TRUE)
        else()
            message(VERBOSE 
                "Config file of ${libName} not found.")
        endif()
    endif()

    #--------------------------------------------------------------------------
    # Resort to manual specification if all means failed
    #--------------------------------------------------------------------------
    if(NOT ${libName}_LOADED)
        message(STATUS 
            "Entering manual ${libName} library specification mode.")
            
        set(MANUAL_${libName}_INC_DIRS  "" CACHE PATH 
            "${libName} library's include directories.")
        set(MANUAL_${libName}_LIB_DIRS  "" CACHE PATH 
            "${libName} library's library directories.")
        set(MANUAL_${libName}_LIB_NAMES "" CACHE PATH 
            "${libName} library's library names.")

        # Possibly override input arguments
        if(MANUAL_${libName}_INC_DIRS)
            set(ARG_MANUAL_INC_DIRS ${MANUAL_${libName}_INC_DIRS})
        endif()
        if(MANUAL_${libName}_INC_DIRS)
            set(ARG_MANUAL_LIB_DIRS ${MANUAL_${libName}_LIB_DIRS})
        endif()
        if(MANUAL_${libName}_LIB_NAMES)
            set(ARG_MANUAL_LIB_NAMES ${MANUAL_${libName}_LIB_NAMES})
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
                if(NOT ARG_OPTIONAL)
                    message(WARNING 
                        "Library ${LIB_NAME_SEMICOLON_LIST} not found, please specify its location via ${libName}_LIB_${LIB_NAME_WITHOUT_SPACES}.")
                endif()
            endif()
        endforeach()

        set(MANUAL_${libName}_INCLUDES  ${ARG_MANUAL_INC_DIRS} PARENT_SCOPE)
        set(MANUAL_${libName}_LIBRARIES ${LIBS_LIST}           PARENT_SCOPE)

        set(${libName}_LOAD_MODE "MANUAL")
        # TODO: check include dir exists
        if(ALL_LIBS_FOUND AND ARG_MANUAL_INC_DIRS)
            set(${libName}_LOADED TRUE)
        endif()
    endif()

    #--------------------------------------------------------------------------
    # Finally, expose information about how the library is loaded
    #--------------------------------------------------------------------------
    if(${libName}_LOADED)
        message(STATUS
            "Library ${libName} loaded.")
        set(${libName}_LOAD_MODE ${${libName}_LOAD_MODE} PARENT_SCOPE)
        set(${libName}_LOADED    ${${libName}_LOADED}    PARENT_SCOPE)
    elseif(NOT ARG_OPTIONAL)
        message(FATAL_ERROR
            "Unable to load library ${libName}.")
    endif()
endfunction()
