from utility import filesystem

import shutil
import os
import configparser


def get_photon_blend_project_directory(setup_config: configparser.ConfigParser):
    return os.path.abspath(setup_config["PhotonBlend"]["ProjectDirectory"])

def get_photon_blend_src_directory(setup_config: configparser.ConfigParser):
    project_dir = get_photon_blend_project_directory(setup_config)
    subdir = setup_config["PhotonBlend"]["SourceDirectory"]
    return os.path.abspath(os.path.join(project_dir, subdir))

def get_photon_blend_generated_code_directory(setup_config: configparser.ConfigParser):
    project_dir = get_photon_blend_project_directory(setup_config)
    subdir = setup_config["PhotonBlend"]["GeneratedCodeDirectory"]
    return os.path.abspath(os.path.join(project_dir, subdir))

def get_photon_blend_install_directory(setup_config: configparser.ConfigParser):
    build_dir = os.path.abspath(setup_config["General"]["BuildDirectory"])
    subdir = setup_config["PhotonBlend"]["InstallLocation"]
    return os.path.abspath(os.path.join(build_dir, subdir))

def setup_photon_blend(setup_config: configparser.ConfigParser):
    src_dir = get_photon_blend_src_directory(setup_config)
    install_dir = get_photon_blend_install_directory(setup_config)

    # Delete old installed content first if it exists (basically a clean install)
    if filesystem.delete_folder_with_contents(install_dir):
        print("Old installed content deleted")

    src_folder_name = os.path.basename(src_dir)
    if not src_folder_name:
        # May be empty if the path to directory contains a trailing slash,
        # in such case, split first to get rid of the slash
        src_folder_name = os.path.basename(os.path.split(src_dir)[0])

    print("Installing Blender addon: %s" % src_folder_name)

    shutil.copytree(src_dir, install_dir, dirs_exist_ok=True)
