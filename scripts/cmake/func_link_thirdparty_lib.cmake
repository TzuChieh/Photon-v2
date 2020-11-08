# TODO: allow to specify link scope
function(link_thirdparty_lib targetName libName)
    set(options OPTIONAL)
    set(oneValueArgs)
    set(multiValueArgs)
    cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
    
    if(NOT ${libName}_LOADED)
        if(NOT ARG_OPTIONAL)
            message(FATAL_ERROR 
                "Cannot link ${targetName} against ${libName}, please make sure load_thirdparty_lib() is correctly invoked.")
        endif()
        return()
    endif()

    if(${${libName}_LOAD_MODE} STREQUAL "VARS")
        target_include_directories(${targetName} PRIVATE ${VARS_${libName}_INCLUDE_DIRS})
        target_link_libraries(${targetName} PRIVATE ${VARS_${libName}_LIBRARIES})
    elseif(${${libName}_LOAD_MODE} STREQUAL "MODULE")
        target_link_libraries(${targetName} PRIVATE ${MODULE_${libName}_TARGETS})
    elseif(${${libName}_LOAD_MODE} STREQUAL "PKG")
        target_link_libraries(${targetName} PRIVATE ${PKG_${libName}_TARGETS})
    elseif(${${libName}_LOAD_MODE} STREQUAL "CONFIG")
        target_link_libraries(${targetName} PRIVATE ${CONFIG_${libName}_TARGETS})
    elseif(${${libName}_LOAD_MODE} STREQUAL "MANUAL")
        target_include_directories(${targetName} PRIVATE ${MANUAL_${libName}_INCLUDES})
        target_link_libraries(${targetName} PRIVATE ${MANUAL_${libName}_LIBRARIES})
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
