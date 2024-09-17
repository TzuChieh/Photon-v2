import content.doxygen
import library_downloader
import resource_downloader
import blender_addon
import render_test
import content
from utility import config
from utility import console

import sys
import shutil
import argparse
from pathlib import Path


# Requires Python version >= 3.9, check that if we have the required version
print("Python version: %s" % sys.version)
if (int(sys.version_info[0]), int(sys.version_info[1])) < (3, 9):
    print("Require Python version >= 3.9")
    sys.exit()

parser = argparse.ArgumentParser(description="Photon Renderer Setup Script")
parser.add_argument('-d', '--directory', type=str, help="Build directory.")
parser.add_argument('--skipdl', action=argparse.BooleanOptionalAction, help="Skip all download steps.")
args = parser.parse_args()

# Read and parse setup config
setup_config = config.get_setup_config()

# Use the build directory as specified via config, optionally overridden by command line arguments
build_dir = Path(args.directory).absolute() if args.directory is not None else None
config_build_dir = Path(setup_config["General"]["BuildDirectory"]).absolute()
if build_dir is not None and build_dir.resolve() != config_build_dir.resolve():
    print("Overriding build directory to <%s> (was <%s>)" % (
        config_build_dir, build_dir))
else:
    build_dir = config_build_dir
    
build_dir.mkdir(parents=True, exist_ok=True)
print(f"Using build directory: {build_dir}")

# Download additional data to build directory
if not args.skipdl:
    library_downloader.download_thirdparty_library(build_dir, setup_config)
    resource_downloader.download_external_resource(build_dir)

# Setup libraries (for now, only non-Windows platforms need this)
print("Setup libraries...")
if sys.platform != 'win32':
    lib_setup_msg = console.run_python_from(build_dir / "Photon-v2-ThirdParty", "./Prebuilt/unpack.py")
    print(lib_setup_msg)

# Setup Blender addon
blender_addon.setup_photon_blend(setup_config)

# Setup render test
render_test.setup_render_test(setup_config)

# Install project source data to build directory

src_dst_dirs = []

dst_config_root = build_dir / "Config"
dst_internal_res_root = build_dir / "InternalResource"
dst_script_root = build_dir / "Script"

# Gather project data to install
for section_name, section in config.get_all_projects(setup_config):
    project_name = section_name.removeprefix("Project.")
    print("Gathering source data from %s" % project_name)

    project_dir = Path(section["ProjectDirectory"])

    if setup_config.has_option(section_name, "ConfigDirectory"):
        src_config_dir = project_dir / section["ConfigDirectory"]
        dst_config_dir = dst_config_root / project_name
        src_dst_dirs.append((src_config_dir, dst_config_dir))

    if setup_config.has_option(section_name, "InternalResourceDirectory"):
        src_internal_res_dir = project_dir / section["InternalResourceDirectory"]
        dst_internal_res_dir = dst_internal_res_root / project_name
        src_dst_dirs.append((src_internal_res_dir, dst_internal_res_dir))

    if setup_config.has_option(section_name, "ScriptDirectory"):
        src_script_dir = project_dir / section["ScriptDirectory"]
        dst_script_dir = dst_script_root / project_name
        src_dst_dirs.append((src_script_dir, dst_script_dir))

# Gather miscellaneous data to install

# Place main configs directly into the installed config directory
src_dst_dirs.append(("./Main/Config/", dst_config_root))

print("Installing source data")
for src_dir, dst_dir in src_dst_dirs:
    shutil.copytree(src_dir, dst_dir, dirs_exist_ok=True)

# Create generated in-source contents

# Create a `Generated` folder for each project to contain generated in-source contents
for section_name, section in config.get_all_projects(setup_config):
    project_dir = Path(section["ProjectDirectory"])
    (project_dir / "Generated").mkdir(exist_ok=True)

# Create a `Base_doxygen.config` with common settings for each project that produces docs
for section_name, section in config.get_all_projects(setup_config):
    # Projects without a config do not produce docs
    if not setup_config.has_option(section_name, "DoxygenConfig"):
        continue

    project_dir = Path(section["ProjectDirectory"])

    content.doxygen.create_base_config(
        project_dir / "Generated",
        project_dir,
        section["ProjectName"],
        section_name.removeprefix("Project."))

# Perform miscellaneous operations

# Create a `docs` sub-folder under `./Main/` so doxygen can place generated docs under `./Main/docs/<project-name>/`
# (a workaround since doxygen cannot create directories recursively, only the topmost layer)
Path("./Main/docs/").mkdir(exist_ok=True)
