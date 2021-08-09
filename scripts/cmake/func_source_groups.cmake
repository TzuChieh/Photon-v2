#--------------------------------------------------------------------------
# Mostly for working with Visual Studio, where it is desirable to create
# source file filters automatically. CMake's source gorup commands can do
# the job for us.
#
# Note that the source file paths provided must match the ones passed to
# add_library() or add_executable(), or other related calls.
#--------------------------------------------------------------------------
function(make_source_groups_from_file_paths)

    cmake_policy(SET CMP0074 NEW)

    set(OPTIONS)

    set(ONE_VALUE_ARGS
        GROUP_ROOT)

    set(MULTI_VALUE_ARGS
        FILE_PATHS)
    
    cmake_parse_arguments(ARG "${OPTIONS}" "${ONE_VALUE_ARGS}" "${MULTI_VALUE_ARGS}" ${ARGN})
    
    foreach(FILE_PATH_LIST ${ARG_FILE_PATHS})
        foreach(FILE_PATH ${FILE_PATH_LIST})
            # Get the containing folder of the source file
            get_filename_component(FILE_DIRECTORY "${FILE_PATH}" DIRECTORY)

            # Get a relative-path representation of the folder
            file(RELATIVE_PATH FILE_DIRECTORY_RELATIVE "${ARG_GROUP_ROOT}" "${FILE_DIRECTORY}")

            # Transform to a Windows-fashioned path
            string(REPLACE "/" "\\" GROUP_DIRECTORY_RELATIVE "${FILE_DIRECTORY_RELATIVE}")

            # Finally make the group
            source_group("${GROUP_DIRECTORY_RELATIVE}" FILES "${FILE_PATH}")

            # message(STATUS ${GROUP_DIRECTORY_RELATIVE})
            # message(STATUS ${FILE_PATH})
        endforeach()
    endforeach()
    
endfunction()
