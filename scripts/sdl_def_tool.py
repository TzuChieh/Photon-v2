from utility import config
import blender_addon

import argparse
import subprocess
from pathlib import Path


class GeneratedSource:
    def __init__(self):
        self.class_name = ""
        self.sdl_impl = ""

def generate_source_for(header_source: str):
    macro_name = 'PH_DEFINE_SDL_FUNCTION_'
    macro_prefix = macro_name + '('

    results = []
    for line in header_source.splitlines():
        line = line.strip()
        if not line.startswith(macro_prefix):
            continue

        tokens = line[len(macro_prefix):].split(',')

        # We need at least `CppOwnerType, funcDef`
        if len(tokens) < 2:
            raise ValueError(f"{macro_name}() requires at least 2 arguments, {len(tokens)} were given")

        result = GeneratedSource()
        result.class_name = tokens[0].strip()
        result.sdl_impl = ""# TODO

    return results

    
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

parser = argparse.ArgumentParser(description="PSDL Definition Tool")
args = parser.parse_args()

# Gather project directories from config
generated_sources = []
for name, section in config.get_all_projects(config.get_setup_config()):
    project_name = name.removeprefix("Project.")
    source_dir = Path(section["ProjectDirectory"]) / "Source"
    for dirpath, dirnames, filenames in source_dir.walk():
        for filename in filenames:
            if not filename.endswith(".h"):
                continue
            header_source = (dirpath / filename).read_text()
            generated_sources.extend(generate_source_for(header_source))

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
