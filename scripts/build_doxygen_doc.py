from utility import config
import blender_addon

import argparse
import subprocess
from pathlib import Path


def build_doxygen_doc(doxygen_executable, doxygen_config):
    working_dir = Path(doxygen_config).parent
    doxygen_config_name = Path(doxygen_config).name

    command_args = [Path(doxygen_executable).absolute()]
    command_args.append(doxygen_config_name)

    # The convention is to run doxygen in the same directory as the config file
    command_result = subprocess.run(
        command_args, 
        cwd=working_dir)

    if command_result.returncode != 0:
        print("command <%s> ran with error (error code: %s)" % 
            (", ".join([str(arg) for arg in command_args]), str(command_result.returncode)))

parser = argparse.ArgumentParser(description="Photon Documentation Builder")
parser.add_argument(
    'doxygen_executable', type=str, help="Path to the Doxygen executable.")
parser.add_argument(
    '-b', '--build', type=str, default="", help=("Specify a specific project to build by its name. Use \"primary\" "
    "to build the primary documentation that links all the other documentations."))
args = parser.parse_args()

doxygen_executable = args.doxygen_executable
specific_project_name = args.build
setup_config = config.get_setup_config()

# Gather doc build info from projects
projects = []
if not specific_project_name:
    projects.extend(config.get_all_projects(setup_config))
elif specific_project_name != "primary":
    projects.append(config.get_project(setup_config, specific_project_name))

# Build doc for projects
build_info = []
for name, section in projects:
    project_name = name.removeprefix("Project.")
    if setup_config.has_option(name, "DoxygenConfig"):
        project_dir = Path(section["ProjectDirectory"])
        doxygen_config = project_dir / section["DoxygenConfig"]
        build_doxygen_doc(doxygen_executable, doxygen_config)
        build_info.append("Generated doc for: %s" % project_name)
    else:
        build_info.append("Project \"%s\" has no doxygen config, no doc generated." % project_name)
    
# Build doc for PhotonBlend
photon_blend_project_dir = Path(blender_addon.get_photon_blend_project_dir(setup_config))# FIXME #94: return Path directly
build_doxygen_doc(doxygen_executable, photon_blend_project_dir / "doxygen.config")
build_info.append("Generated doc for PhotonBlend")

# Build the primary doc that links all docs
primary_doxygen_config = setup_config["General"]["PrimaryDoxygenConfig"]
build_doxygen_doc(doxygen_executable, primary_doxygen_config)
build_info.append("Primary doc generated.")

# Print info for generated docs
print("============ Done Building Docs ============")
for info in build_info:
    print(info)
