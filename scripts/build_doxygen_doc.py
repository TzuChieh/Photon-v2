from utility import console
from utility import config

import sys
import os
import subprocess


def build_doxygen_doc(doxygen_executable, doxygen_config):
    working_directory = os.path.dirname(doxygen_config)
    doxygen_config_name = os.path.basename(doxygen_config)

    # The convention is to run doxygen in the same directory as the config file
    command_args = [str(os.path.abspath(doxygen_executable))]
    command_args.append(doxygen_config_name)

    command_result = subprocess.run(
        command_args, 
        cwd=working_directory)

    if command_result.returncode != 0:
        print("command <%s> ran with error (error code: %s)" % 
            (", ".join([str(arg) for arg in command_args]), str(command_result.returncode)))

doxygen_executable = sys.argv[1]
specific_project_name = sys.argv[2] if len(sys.argv) > 2 else ""
specific_project_name = specific_project_name.removeprefix("--")
setup_config = config.get_setup_config()

projects = []
if specific_project_name != "primary":
    # Build doc for a selected project only
    if specific_project_name:
        projects.append(config.get_project(setup_config, specific_project_name))
    # Build doc for all projects
    else:
        projects.extend(config.get_all_projects(setup_config))

build_info = []
for name, section in projects:
    project_name = name.removeprefix("Project.")
    if setup_config.has_option(name, "DoxygenConfig"):
        doxygen_config = os.path.join(section["ProjectDirectory"], section["DoxygenConfig"])
        build_doxygen_doc(doxygen_executable, doxygen_config)
        build_info.append("Generated doc for: %s" % project_name)
    else:
        build_info.append("Project \"%s\" has no doxygen config, no doc generated." % project_name)
    
# Build the primary doc that links all project docs
primary_doxygen_config = setup_config["General"]["PrimaryDoxygenConfig"]
build_doxygen_doc(doxygen_executable, primary_doxygen_config)
build_info.append("Primary doc generated.")

# Print info for generated docs
print("============ Done Building Docs ============")
for info in build_info:
    print(info)
