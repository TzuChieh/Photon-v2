import content.doxygen
import library_downloader
import resource_downloader
import blender_addon
import render_test
import content
import sdl_def_tool
from utility import config
from utility import console
from utility import filesystem

import sys
import shutil
import argparse
import subprocess
from pathlib import Path


def _prepare_python_env(args, build_dir: Path):
    version = args.py_ver
    runtime_version = (int(sys.version_info[0]), int(sys.version_info[1]))

    target_version = runtime_version
    if version:
        target_version = tuple(int(t) for t in version.split('.'))

    # Requires Python version >= 3.9, check that if we have the required version
    print("Python version: %s" % sys.version)
    if runtime_version < (3, 9):
        raise ValueError("Require Python version >= 3.9")

    # Re-run the script with target version
    if target_version and target_version != runtime_version:
        if sys.platform == 'win32':
            print(f"Switching to Python {version}...")

            fwd_args = ['py', f'-{target_version[0]}.{target_version[1]}'] + sys.argv
            result = subprocess.run(fwd_args)
            if result.returncode != 0:
                raise ValueError(f"Version switch failed. Please install Python {version}.")
            
            # We are replaced by the subprocess spawned earlier
            sys.exit(result.returncode)
        else:
            print("warning: Version switch is not implemented. Using system Python.")

    # Create virtual environment for target version
    if args.py_env:
        venv_dir = build_dir / 'ApplicationEnv'
        print(f"Creating virtual environment at {venv_dir}...")
        if filesystem.delete_folder_with_contents(venv_dir):
            print(f"Removed previous virtual environment at <{venv_dir}>")
        
        # Make sure previous venv is removed before creating a new one
        if not venv_dir.exists():
            console.run_python('-m', 'venv', venv_dir)
        else:
            raise ValueError(f"Unable to remove previous virtual environment <{venv_dir}>. Terminate it and run setup again.")

        # Skips remaining steps as promised by `--py-env`
        sys.exit(0)

# Read and parse setup config
setup_config = config.get_setup_config()

parser = argparse.ArgumentParser(description="Photon Renderer Setup Script")
parser.add_argument('-d', '--directory', type=str, help="Build directory.")
parser.add_argument('--skip-dl', action=argparse.BooleanOptionalAction, help="Skip all download steps.")
parser.add_argument('--py-ver', type=str, help="Specify the Python version to use (e.g., 3.10).")
parser.add_argument('--py-env', action=argparse.BooleanOptionalAction, help="Setup Python environment only and skip other steps. Respects --py-ver.")
args = parser.parse_args()

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

_prepare_python_env(args, build_dir)

# Download additional data to build directory
if not args.skip_dl:
    library_downloader.download_thirdparty_library(build_dir, setup_config)
    resource_downloader.download_external_resource(build_dir)

# Setup libraries (for now, only non-Windows platforms need this)
print("Setting up libraries...")
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

print("Installing source data...")
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
    
# Create SDL definitions
print("Generating SDL definitions...")
sdl_def_tool.generate(setup_config)

# Perform miscellaneous operations

# Create a `docs` sub-folder under `./Main/` so doxygen can place generated docs under `./Main/docs/<project-name>/`
# (a workaround since doxygen cannot create directories recursively, only the topmost layer)
Path("./Main/docs/").mkdir(exist_ok=True)
