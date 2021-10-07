import shutil
import os
import configparser


def get_photon_blend_src_directory(setup_config: configparser.ConfigParser):
    return os.path.abspath(setup_config["PhotonBlend"]["SourceDirectory"])

def get_photon_blend_generated_code_directory(setup_config: configparser.ConfigParser):
    return os.path.abspath(setup_config["PhotonBlend"]["GeneratedCodeDirectory"])

def get_photon_blend_install_directory(setup_config: configparser.ConfigParser):
    build_directory = os.path.abspath(setup_config["Paths"]["BuildDirectory"])
    return os.path.join(build_directory, setup_config["PhotonBlend"]["InstallLocation"])

def setup_photon_blend(setup_config: configparser.ConfigParser):

    src_directory = get_photon_blend_src_directory(setup_config)
    install_directory = get_photon_blend_install_directory(setup_config)

    src_folder_name = os.path.basename(src_directory)
    if not src_folder_name:
        # May be empty if the path to directory contains a trailing slash,
        # in such case, split first to get rid of the slash
        src_folder_name = os.path.basename(os.path.split(src_directory)[0])

    print("Installing Blender addon: %s" % src_folder_name)

    shutil.copytree(src_directory, install_directory, dirs_exist_ok=True)
