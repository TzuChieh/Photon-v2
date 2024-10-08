# 
# @param targetName Target to link against the third-party library.
# @param libName The third-party library to link.
# @param DETECTING_PREPROCESSOR [1-arg] Name of the preprocessor macro to define.
# Defined to 1 if library is linked, otherwise defined to 0. Note that there should 
# be no space in the name of the preprocessor macro.
# @param PRIVATE [0-arg] Link as private library. This is the default if not specified.
# @param PUBLIC [0-arg] Link as public library.
# @param INTERFACE [0-arg] Link as interface library.
#
function(link_thirdparty_lib targetName libName)
    set(OPTIONS 
        OPTIONAL
        PUBLIC
        PRIVATE
        INTERFACE)

    set(ONE_VALUE_ARGS
        DETECTING_PREPROCESSOR)

    set(MULTI_VALUE_ARGS)

    cmake_parse_arguments(ARG "${OPTIONS}" "${ONE_VALUE_ARGS}" "${MULTI_VALUE_ARGS}" ${ARGN})
    
    if(ARG_PUBLIC)
        set(LIB_ACCESSIBILITY "PUBLIC")
    elseif(ARG_PRIVATE)
        set(LIB_ACCESSIBILITY "PRIVATE")
    elseif(ARG_INTERFACE)
        set(LIB_ACCESSIBILITY "INTERFACE")
    else()
        # `PRIVATE` by default
        set(LIB_ACCESSIBILITY "PRIVATE")
    endif()

    # Optionally define a preprocessor macro for detecting the library
    if(ARG_DETECTING_PREPROCESSOR)
        if(${libName}_LOADED)
            target_compile_definitions(${targetName} ${LIB_ACCESSIBILITY} ${ARG_DETECTING_PREPROCESSOR}=1)
        else()
            target_compile_definitions(${targetName} ${LIB_ACCESSIBILITY} ${ARG_DETECTING_PREPROCESSOR}=0)
        endif()
    endif()

    if(NOT ${libName}_LOADED)
        if(NOT ARG_OPTIONAL)
            message(FATAL_ERROR 
                "Cannot link ${targetName} against ${libName}, please make sure load_thirdparty_lib() is correctly invoked.")
        endif()
        return()
    endif()

    if(${${libName}_LOAD_MODE} STREQUAL "VARS")
        target_include_directories(${targetName} ${LIB_ACCESSIBILITY} ${VARS_${libName}_INCLUDE_DIRS})
        target_link_libraries(${targetName} ${LIB_ACCESSIBILITY} ${VARS_${libName}_LIBRARIES})
    elseif(${${libName}_LOAD_MODE} STREQUAL "MODULE")
        target_link_libraries(${targetName} ${LIB_ACCESSIBILITY} ${MODULE_${libName}_TARGETS})
    elseif(${${libName}_LOAD_MODE} STREQUAL "PKG")
        target_link_libraries(${targetName} ${LIB_ACCESSIBILITY} ${PKG_${libName}_TARGETS})
    elseif(${${libName}_LOAD_MODE} STREQUAL "CONFIG")
        target_link_libraries(${targetName} ${LIB_ACCESSIBILITY} ${CONFIG_${libName}_TARGETS})
    elseif(${${libName}_LOAD_MODE} STREQUAL "MANUAL")
        target_include_directories(${targetName} ${LIB_ACCESSIBILITY} ${MANUAL_${libName}_INCLUDES})
        target_link_libraries(${targetName} ${LIB_ACCESSIBILITY} ${MANUAL_${libName}_LIBRARIES})
    elseif(NOT ARG_OPTIONAL)
        message(FATAL_ERROR
            "Cannot link ${targetName} against ${libName}, unknown load mode <${${libName}_LOAD_MODE}> detected.")
    endif()

    if(${libName}_RUNTIME_DIR)
        if(EXISTS "${${libName}_RUNTIME_DIR}")
            # TODO: no copy if no change, and option to force copy
            # TODO: `add_custom_command()` supports generator expression
            file(COPY
                "${${libName}_RUNTIME_DIR}"
                DESTINATION ${BUILD_OUTPUT_DIR})

            # TODO: better if we can just copy what debug build needs (e.g., reading target property for dll location)
            file(COPY
                "${${libName}_RUNTIME_DIR}"
                DESTINATION ${DEBUG_BUILD_OUTPUT_DIR})
        else()
            message(STATUS 
                "Skipping copy of runtime: directory \"${${libName}_RUNTIME_DIR}\" does not exist.")
        endif()
    endif()
endfunction()
