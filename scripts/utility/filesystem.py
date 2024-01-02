import os
import shutil
import time
import warnings


def delete_folder_with_contents(folder_path):
    """
    @brief Delete the folder, all contents inside are also deleted recursively.
    @return Whether the folder is deleted.
    """
    print("Deleting folder <%s>..." % folder_path)

    if not os.path.isdir(folder_path):
        return False

    max_retries = 10
    num_retries = 0
    while num_retries < max_retries:
        try:
            # Errors can be generated such as there are read-only files (the `ignore_errors`` option can help with that)
            shutil.rmtree(folder_path)
        except OSError:
            # Errors might be generated inside `shutil.rmtree()` if the folder-to-delete is currently opened by
            # the file explorer. Wait a short time to give it a chance to close itself before we retry.
            # [1] https://bugs.python.org/issue33240.
            # [2] https://stackoverflow.com/questions/33656696/python-throws-error-when-deleting-a-directory-that-is-open-with-windows-explorer
            time.sleep(0.5)

            # Retry if it still exists
            if os.path.isdir(folder_path):
                num_retries = num_retries + 1
            else:
                break

    if num_retries == max_retries:
        warnings.warn("Cannot delete folder <%s>" % folder_path, stacklevel=16)

    return not os.path.isdir(folder_path)

def rename_folder(src_folder_path, dst_folder_path):
    """
    @brief Rename the folder.
    @return Whether the folder is renamed.
    """

    if not os.path.isdir(src_folder_path):
        return False
    
    max_retries = 10
    num_retries = 0
    while num_retries < max_retries:
        try:
            os.rename(src_folder_path, dst_folder_path)
        except OSError:
            # Deal with the issue similar to `delete_folder_with_contents()`, see also
            # https://stackoverflow.com/questions/37830326/how-to-avoid-windowserror-error-5-access-is-denied
            # https://github.com/conan-io/conan/issues/6560
            time.sleep(0.5)

            # Retry if it still exists
            if os.path.isdir(src_folder_path):
                num_retries = num_retries + 1
            else:
                break

    if num_retries == max_retries:
        warnings.warn("Cannot rename folder <%s> to <%s>" % (src_folder_path, dst_folder_path), stacklevel=16)

    return not os.path.isdir(src_folder_path) and os.path.isdir(dst_folder_path)
