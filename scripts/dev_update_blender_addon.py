import blender_addon
from utility import console
from utility import config

import sys
from pathlib import Path


# First obtain the setup config
setup_config = config.get_setup_config()

build_dir = Path(setup_config["General"]["BuildDirectory"]).absolute()

# Obtain path to the SDLGenCLI executable
sdl_gen_cli_executable_name = "SDLGenCLI.exe"
if sys.platform != "win32":
    sdl_gen_cli_executable_name = "SDLGenCLI"
sdl_gen_cli_path = build_dir / "bin" / sdl_gen_cli_executable_name
print(f"SDLGenCLI in use: {sdl_gen_cli_path}")

# Call SDLGenCLI to generate pysdl.py

pysdl_output_dir = blender_addon.get_photon_blend_generated_code_dir(setup_config)
print(f"Output folder of pysdl.py: {pysdl_output_dir}")

command_result = console.run_command_from(
    build_dir, 
    sdl_gen_cli_path, 
    "--interface", "python",
    "--output", pysdl_output_dir)

print(f"SDLGenCLI output: {command_result}")

# Setup the addon
blender_addon.setup_photon_blend(setup_config)
