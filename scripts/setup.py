import library_downloader
import resource_downloader

import sys
import os
import configparser
import shutil


# Requires Python version >= 3.8, check that if we have the required version
if sys.version_info[0] < 3 or sys.version_info[1] < 8:
    print("Require Python version >= 3.8, we have %s" % sys.version)
    sys.exit()

build_directory = os.path.abspath(sys.argv[1])
setup_script_directory = os.path.dirname(os.path.abspath(__file__))
setup_config_path = os.path.join(setup_script_directory, "SetupConfig.ini")

# Read and parse setup config
print("Loading setup settings from <%s>" % setup_config_path)
setup_config = configparser.ConfigParser()
setup_config.read_file(open(setup_config_path))

config_build_directory = os.path.abspath(setup_config["Paths"]["BuildDirectory"])
if not os.path.samefile(config_build_directory, build_directory):
    print("Using build directory other than <%s> is not recommended (currently <%s>)" % (
        config_build_directory, build_directory))

# Download additional data to build directory
library_downloader.download_thirdparty_library(build_directory)
resource_downloader.download_external_resource(build_directory)

# Install source data to build directory

project_names = [
    "Engine",
    "EngineTest",
    "SDLGen",
    "SDLGenCLI",
    "PhotonCLI",
    "PhotonEditor",
]

dst_config_root = os.path.join(build_directory, "Config")
dst_internal_res_root = os.path.join(build_directory, "InternalResource")

src_dst_directories = []
for project_name in project_names:
    if not setup_config.has_section(project_name):
        continue

    print("Gathering source data from %s" % project_name)

    if setup_config.has_option(project_name, "ConfigDirectory"):
        src_config_directory = os.path.abspath(setup_config[project_name]["ConfigDirectory"])
        dst_config_directory = os.path.join(dst_config_root, project_name)
        src_dst_directories.append((src_config_directory, dst_config_directory))

    if setup_config.has_option(project_name, "InternalResourceDirectory"):
        src_internal_res_directory = os.path.abspath(setup_config[project_name]["InternalResourceDirectory"])
        dst_internal_res_directory = os.path.join(dst_internal_res_root, project_name)
        src_dst_directories.append((src_internal_res_directory, dst_internal_res_directory))

print("Installing source data")

for src_dst_directory in src_dst_directories:
    src_directory = src_dst_directory[0]
    dst_directory = src_dst_directory[1]
    shutil.copytree(src_directory, dst_directory, dirs_exist_ok=True)

