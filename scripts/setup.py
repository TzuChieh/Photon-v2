import library_downloader
import resource_downloader
import blender_addon
import render_test
from utility import config

import sys
import os
import shutil


# Requires Python version >= 3.9, check that if we have the required version
print("Python version: %s" % sys.version)
if (int(sys.version_info[0]), int(sys.version_info[1])) < (3, 9):
    print("Require Python version >= 3.9")
    sys.exit()

build_dir = os.path.abspath(sys.argv[1])

# Read and parse setup config
setup_config = config.get_setup_config()

config_build_dir = os.path.abspath(setup_config["General"]["BuildDirectory"])
if not os.path.samefile(config_build_dir, build_dir):
    print("Using build directory other than <%s> is not recommended (currently <%s>)" % (
        config_build_dir, build_dir))

# Download additional data to build directory
library_downloader.download_thirdparty_library(build_dir, setup_config)
resource_downloader.download_external_resource(build_dir)

# Setup Blender addon
blender_addon.setup_photon_blend(setup_config)

# Setup render test
render_test.setup_render_test(setup_config)

# Install project source data to build directory

src_dst_dirs = []

dst_config_root = os.path.join(build_dir, "Config")
dst_internal_res_root = os.path.join(build_dir, "InternalResource")
dst_script_root = os.path.join(build_dir, "Script")

# Gather project data to install
for section_name, section in config.get_all_projects(setup_config):
    project_name = section_name.removeprefix("Project.")
    print("Gathering source data from %s" % project_name)

    project_dir = section["ProjectDirectory"]

    if setup_config.has_option(section_name, "ConfigDirectory"):
        src_config_dir = os.path.join(project_dir, section["ConfigDirectory"])
        dst_config_dir = os.path.join(dst_config_root, project_name)
        src_dst_dirs.append((src_config_dir, dst_config_dir))

    if setup_config.has_option(section_name, "InternalResourceDirectory"):
        src_internal_res_dir = os.path.join(project_dir, section["InternalResourceDirectory"])
        dst_internal_res_dir = os.path.join(dst_internal_res_root, project_name)
        src_dst_dirs.append((src_internal_res_dir, dst_internal_res_dir))

    if setup_config.has_option(section_name, "ScriptDirectory"):
        src_script_dir = os.path.join(project_dir, section["ScriptDirectory"])
        dst_script_dir = os.path.join(dst_script_root, project_name)
        src_dst_dirs.append((src_script_dir, dst_script_dir))

# Gather miscellaneous data to install

# Place main configs directly into the installed config directory
src_dst_dirs.append(("./Main/Config/", dst_config_root))

print("Installing source data")
for src_dir, dst_dir in src_dst_dirs:
    shutil.copytree(src_dir, dst_dir, dirs_exist_ok=True)

# Perform miscellaneous operations

# Create a `docs` sub-folder under `./Main/` so doxygen can place generated docs under `./Main/docs/<project-name>/`
# (a workaround since doxygen cannot create directories recursively, only the topmost layer)
os.makedirs("./Main/docs/", exist_ok=True)
