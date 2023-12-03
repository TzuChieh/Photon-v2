import blender_addon
from utility import console

import configparser
import os
import sys


# First obtain the setup config
setup_script_directory = os.path.dirname(os.path.abspath(__file__))
setup_config_path = os.path.join(setup_script_directory, "../Main/SetupConfig.ini")
setup_config = configparser.ConfigParser()
setup_config.read_file(open(setup_config_path))

build_directory = os.path.abspath(setup_config["General"]["BuildDirectory"])

# Obtain path to the SDLGenCLI executable
sdl_gen_cli_executable_name = "SDLGenCLI.exe"
if sys.platform != "win32":
    sdl_gen_cli_executable_name = "SDLGenCLI"
sdl_gen_cli_path = os.path.join(build_directory, "bin", sdl_gen_cli_executable_name)
print("SDLGenCLI in use: %s" % sdl_gen_cli_path)

# Call SDLGenCLI to generate pysdl.py

pysdl_output_directory = blender_addon.get_photon_blend_generated_code_directory(setup_config)
print("Output folder of pysdl.py: %s" % pysdl_output_directory)

command_result = console.run_command_from(
    build_directory, 
    sdl_gen_cli_path, 
    "--interface", "python",
    "--output", pysdl_output_directory)

print("SDLGenCLI output: %s" % command_result)

# Setup the addon
blender_addon.setup_photon_blend(setup_config)
