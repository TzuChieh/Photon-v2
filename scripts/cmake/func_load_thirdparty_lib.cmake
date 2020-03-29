# TODO: make config path a multivalueArg
function(load_thirdparty_lib libName)
    set(options 
        OPTIONAL)
    set(oneValueArgs 
        CONFIG_PATH)
    set(multiValueArgs 
        CONFIG_TARGETS
        MANUAL_INC_DIRS 
        MANUAL_LIB_DIRS 
        MANUAL_LIB_NAMES)
    cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    # TODO: possible vars ignore case?

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
            set(${libName}_INCLUDE_DIRS "${${INCLUDE_DIR_VAR}}" PARENT_SCOPE)
            break()
        endif()
    endforeach()

    foreach(LIB_VAR IN ITEMS ${POSSIBLE_LIB_VARS})
        if(${LIB_VAR})
            set(${libName}_LIBRARIES "${${LIB_VAR}}" PARENT_SCOPE)
            break()
        endif()
    endforeach()

    # First check if existing library variables are properly defined
    if(${libName}_INCLUDE_DIRS AND ${libName}_LIBRARIES)
        message(STATUS 
            "Found existing ${libName} library variables.")
        set(${libName}_LOAD_MODE "VARS" PARENT_SCOPE)

        set(VARS_${libName}_INCLUDE_DIRS "${${libName}_INCLUDE_DIRS}" PARENT_SCOPE)
        set(VARS_${libName}_LIBRARIES    "${${libName}_LIBRARIES}"    PARENT_SCOPE)
    else()
        find_package(${libName} CONFIG QUIET
            PATHS "${ARG_CONFIG_PATH}/"
            NO_DEFAULT_PATH)

        # Try to find library variables defined by find_package()
        if(${libName}_FOUND)
            message(STATUS 
                "Found ${libName} in config mode.")
            set(${libName}_LOAD_MODE "CONFIG" PARENT_SCOPE)

            set(TARGETS_LIST)
            foreach(TARGET_NAME ${ARG_CONFIG_TARGETS})
                list(APPEND TARGETS_LIST "${libName}::${TARGET_NAME}")
            endforeach()
            set(CONFIG_${libName}_TARGETS ${TARGETS_LIST} PARENT_SCOPE)
        # Resort to manual specification if all means failed
        else()
            message(STATUS 
                "Entering manual ${libName} library specification mode.")
            set(${libName}_LOAD_MODE "MANUAL" PARENT_SCOPE)

            set(MANUAL_${libName}_INC_DIRS  ${ARG_MANUAL_INC_DIRS}  CACHE STRING 
                "${libName} library's include directories.")
            set(MANUAL_${libName}_LIB_DIRS  ${ARG_MANUAL_LIB_DIRS}  CACHE STRING 
                "${libName} library's library directories.")
            set(MANUAL_${libName}_LIB_NAMES ${ARG_MANUAL_LIB_NAMES} CACHE STRING 
                "${libName} library's library names.")
            
            set(LIBS_LIST)
            foreach(LIB_NAME IN LISTS MANUAL_${libName}_LIB_NAMES)
                set(LIB_NAME_WITHOUT_SPACES)
                string(REPLACE " " "_OR_" LIB_NAME_WITHOUT_SPACES "${LIB_NAME}")
                
                set(LIB_NAME_SEMICOLON_LIST)
                string(REPLACE " " ";" LIB_NAME_SEMICOLON_LIST "${LIB_NAME}")

                find_library(${libName}_LIB_${LIB_NAME_WITHOUT_SPACES} 
                    NAMES ${LIB_NAME_SEMICOLON_LIST}
                    PATHS ${MANUAL_${libName}_LIB_DIRS}
                    NO_DEFAULT_PATH)

                if(${libName}_LIB_${LIB_NAME_WITHOUT_SPACES})
                    list(APPEND LIBS_LIST ${${libName}_LIB_${LIB_NAME_WITHOUT_SPACES}})
                elseif(NOT ARG_OPTIONAL)
                    message(WARNING 
                        "Library ${LIB_NAME_SEMICOLON_LIST} not found, please specify its location via ${libName}_LIB_${LIB_NAME_WITHOUT_SPACES}.")
                endif()
            endforeach()

            set(MANUAL_${libName}_INCLUDES  ${MANUAL_${libName}_INCLUDE_DIRS} PARENT_SCOPE)
            set(MANUAL_${libName}_LIBRARIES ${LIBS_LIST}                      PARENT_SCOPE)
        endif()
    endif()
endfunction()
