# TODO: allow to specify link scope
function(link_thirdparty_lib targetName libName)
    set(OPTIONS 
        OPTIONAL
        PUBLIC
        PRIVATE
        INTERFACE)

    set(ONE_VALUE_ARGS)
    set(MULTI_VALUE_ARGS)
    cmake_parse_arguments(ARG "${OPTIONS}" "${ONE_VALUE_ARGS}" "${MULTI_VALUE_ARGS}" ${ARGN})
    
    if(NOT ${libName}_LOADED)
        if(NOT ARG_OPTIONAL)
            message(FATAL_ERROR 
                "Cannot link ${targetName} against ${libName}, please make sure load_thirdparty_lib() is correctly invoked.")
        endif()
        return()
    endif()

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

    # TODO: no copy if no change, and option to force copy
    if(${libName}_RUNTIME_DIR)
        file(COPY
            "${${libName}_RUNTIME_DIR}/"
            DESTINATION ${BUILD_OUTPUT_DIR})
    endif()
endfunction()
