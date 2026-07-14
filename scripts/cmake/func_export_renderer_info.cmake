#
# @param OUTPUT_FILE [1-arg] Renderer information file to write.
# @param WORKING_COLOR_SPACE [1-arg] Configured renderer working color space.
#
function(export_renderer_info)

    set(OPTIONS)

    set(ONE_VALUE_ARGS
        OUTPUT_FILE
        WORKING_COLOR_SPACE)

    set(MULTI_VALUE_ARGS)

    cmake_parse_arguments(ARG_INFO "${OPTIONS}" "${ONE_VALUE_ARGS}" "${MULTI_VALUE_ARGS}" ${ARGN})

    if(ARG_INFO_UNPARSED_ARGUMENTS)
        message(FATAL_ERROR
            "export_renderer_info(): unknown arguments: ${ARG_INFO_UNPARSED_ARGUMENTS}")
    endif()
    if(ARG_INFO_KEYWORDS_MISSING_VALUES)
        message(FATAL_ERROR
            "export_renderer_info(): missing values for: ${ARG_INFO_KEYWORDS_MISSING_VALUES}")
    endif()
    if(NOT DEFINED ARG_INFO_OUTPUT_FILE)
        message(FATAL_ERROR "export_renderer_info(): OUTPUT_FILE is required.")
    endif()
    if(NOT DEFINED ARG_INFO_WORKING_COLOR_SPACE)
        message(FATAL_ERROR "export_renderer_info(): WORKING_COLOR_SPACE is required.")
    endif()

    find_package(Git REQUIRED)

    execute_process(
        COMMAND "${GIT_EXECUTABLE}" rev-parse --abbrev-ref HEAD
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
        OUTPUT_VARIABLE BRANCH_NAME
        OUTPUT_STRIP_TRAILING_WHITESPACE
        COMMAND_ERROR_IS_FATAL ANY)
    if(BRANCH_NAME STREQUAL "HEAD")
        set(BRANCH_NAME "(detached HEAD)")
    endif()

    execute_process(
        COMMAND "${GIT_EXECUTABLE}" rev-parse HEAD
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
        OUTPUT_VARIABLE COMMIT_HASH
        OUTPUT_STRIP_TRAILING_WHITESPACE
        COMMAND_ERROR_IS_FATAL ANY)

    set(RENDERER_INFO "{}")
    string(JSON RENDERER_INFO SET "${RENDERER_INFO}" BranchName "\"${BRANCH_NAME}\"")
    string(JSON RENDERER_INFO SET "${RENDERER_INFO}" CommitHash "\"${COMMIT_HASH}\"")
    string(JSON RENDERER_INFO SET "${RENDERER_INFO}" WorkingColorSpace "\"${ARG_INFO_WORKING_COLOR_SPACE}\"")
    file(WRITE "${ARG_INFO_OUTPUT_FILE}" "${RENDERER_INFO}\n")

endfunction()
