import os
import shutil
import time
import warnings

# Returns whether the folder is deleted
def delete_folder_with_contents(folder_path):

    print("Deleting folder <%s>..." % folder_path)

    max_retries = 30
    num_retries = 0
    while num_retries < max_retries:
        # Setting `ignore_errors` so non-empty folders can be deleted without errors being thrown
        # (other errors can also be generated/ignored such as there are read-only files)
        shutil.rmtree(folder_path, ignore_errors=True)

        # Errors might be generated inside `shutil.rmtree()` if the folder-to-delete is currently opened by
        # the file explorer. Wait a short time to give it a chance to close itself before we retry.
        # (FIXME: still occucasionally happens on Win7; updated on 20230513, let's wait and see...)
        # [1] https://bugs.python.org/issue33240.
        # [2] https://stackoverflow.com/questions/33656696/python-throws-error-when-deleting-a-directory-that-is-open-with-windows-explorer
        time.sleep(0.1)

        # Retry if it still exists
        if os.path.isdir(folder_path):
            num_retries = num_retries + 1
        else:
            break

    if num_retries == max_retries:
        warnings.warn( "Cannot delete a non-folder <%s>" % folder_path, stacklevel=16)

    is_folder_deleted = not os.path.isdir(folder_path)
    return is_folder_deleted
