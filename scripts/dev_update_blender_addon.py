import blender_addon
from utility import console

import configparser
import os
import sys


# First obtain the setup config
setup_script_directory = os.path.dirname(os.path.abspath(__file__))
setup_config_path = os.path.join(setup_script_directory, "SetupConfig.ini")

# Setup the addon
blender_addon.setup_photon_blend(setup_config)

build_directory = os.path.abspath(setup_config["Paths"]["BuildDirectory"])

sdl_gen_cli_executable_name = "SDLGenCLI.exe"
if sys.platform != "win32":
    sdl_gen_cli_executable_name = "SDLGenCLI"
sdl_gen_cli_rel_path = os.path.join("./bin", sdl_gen_cli_executable_name)

# TODO: call SDLGen
console.run_command_from(build_directory, )

