from utility import filesystem
from utility import console

import shutil
import os
import configparser
import sys
from pathlib import Path

# FIXME #94: return Path directly

def get_photon_blend_project_dir(setup_config: configparser.ConfigParser):
    return os.path.abspath(setup_config["PhotonBlend"]["ProjectDirectory"])

def get_photon_blend_src_dir(setup_config: configparser.ConfigParser):
    project_dir = get_photon_blend_project_dir(setup_config)
    subdir = setup_config["PhotonBlend"]["SourceDirectory"]
    return os.path.abspath(os.path.join(project_dir, subdir))

def get_photon_blend_generated_code_dir(setup_config: configparser.ConfigParser):
    project_dir = get_photon_blend_project_dir(setup_config)
    subdir = setup_config["PhotonBlend"]["GeneratedCodeDirectory"]
    return os.path.abspath(os.path.join(project_dir, subdir))

def get_photon_blend_install_dir(setup_config: configparser.ConfigParser):
    build_dir = os.path.abspath(setup_config["General"]["BuildDirectory"])
    subdir = setup_config["PhotonBlend"]["InstallLocation"]
    return os.path.abspath(os.path.join(build_dir, subdir))

def setup_photon_blend(setup_config: configparser.ConfigParser):
    """
    Setup (install) PhotonBlend to build directory.
    """
    src_dir = get_photon_blend_src_dir(setup_config)
    install_dir = get_photon_blend_install_dir(setup_config)

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

    # TODO: associate to Blender install (symlink?)

def gen_pysdl_for_photon_blend(setup_config: configparser.ConfigParser):
    """
    Generate `pysdl.py` from SDL definition in C++ code.
    Output to PhotonBlend's generated code directory (in source directory).
    """
    build_dir = Path(setup_config["General"]["BuildDirectory"]).absolute()

    # Obtain path to the `SDLGenCLI` executable
    sdl_gen_cli_executable_name = "SDLGenCLI.exe"
    if sys.platform != "win32":
        sdl_gen_cli_executable_name = "SDLGenCLI"
    sdl_gen_cli_path = build_dir / "bin" / sdl_gen_cli_executable_name
    print(f"Using SDLGenCLI from: {sdl_gen_cli_path}")

    if not sdl_gen_cli_path.is_file():
        raise ValueError(f"Please build first to generate SDLGenCLI executable.")

    # Call SDLGenCLI to generate pysdl.py

    pysdl_output_dir = get_photon_blend_generated_code_dir(setup_config)
    print(f"Output folder of pysdl.py: {pysdl_output_dir}")

    command_result = console.run_command_from(
        build_dir, 
        sdl_gen_cli_path, 
        "--interface", "python",
        "--output", pysdl_output_dir)

    print(f"SDLGenCLI output: {command_result}")

def update_photon_blend(setup_config: configparser.ConfigParser):
    """
    Update PhotonBlend to latest build.
    """
    gen_pysdl_for_photon_blend(setup_config)
    setup_photon_blend(setup_config)
