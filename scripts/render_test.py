from utility import filesystem

import shutil
import os
import configparser


def get_render_test_project_dir(setup_config: configparser.ConfigParser):
    return os.path.abspath(setup_config["RenderTest"]["ProjectDirectory"])

def get_render_test_install_dir(setup_config: configparser.ConfigParser):
    build_dir = os.path.abspath(setup_config["General"]["BuildDirectory"])
    subdir = setup_config["RenderTest"]["InstallLocation"]
    return os.path.abspath(os.path.join(build_dir, subdir))

def setup_render_test(setup_config: configparser.ConfigParser):
    project_dir = get_render_test_project_dir(setup_config)
    install_dir = get_render_test_install_dir(setup_config)

    # Delete old installed content first if it exists (basically a clean install)
    if filesystem.delete_folder_with_contents(install_dir):
        print("Old installed content deleted")

    src_folder_name = os.path.basename(project_dir)
    if not src_folder_name:
        # May be empty if the path to directory contains a trailing slash,
        # in such case, split first to get rid of the slash
        src_folder_name = os.path.basename(os.path.split(project_dir)[0])
    print("Installing render test: %s" % src_folder_name)

    shutil.copytree(project_dir, install_dir, dirs_exist_ok=True)
