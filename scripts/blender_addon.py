import shutil
import os


def setup(addon_source_directory, addon_install_directory):

    abs_src_dir = os.path.abspath(addon_source_directory)
    src_folder_name = os.path.basename(abs_src_dir)
    if not src_folder_name:
        # May be empty if the path to directory contains a trailing slash,
        # in such case, split first to get rid of the slash
        src_folder_name = os.path.basename(os.path.split(abs_src_dir)[0])

    print("Installing Blender addon: %s" % src_folder_name)

    shutil.copytree(addon_source_directory, addon_install_directory, dirs_exist_ok=True)
