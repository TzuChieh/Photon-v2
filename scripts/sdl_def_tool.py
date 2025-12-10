from utility import config
import blender_addon

import argparse
import subprocess
from pathlib import Path


class GeneratedSource:
    def __init__(self):
        self.owner_class_name = ""
        self.sdl_impl = ""

def generate_source_for(header_file: Path, source_dir: Path):
    header_source = (dirpath / filename).read_text()
    
    macro_name = 'PH_DEFINE_SDL_FUNCTION_'
    macro_prefix = macro_name + '('

    results = []
    for line in header_source.splitlines():
        line = line.strip()
        if not line.startswith(macro_prefix):
            continue

        tokens = line[len(macro_prefix):].split(',')

        # For arguments, we need at least `CppOwnerType, funcDef`
        if len(tokens) < 2:
            raise ValueError(f"{macro_name}() requires at least 2 arguments, {len(tokens)} were given")

        owner_class_name = tokens[0].strip()

        result = GeneratedSource()
        result.owner_class_name = owner_class_name

        # Boilerplate for generated C++ source code
        result.sdl_impl = f"""
        #include "{header_file.relative_to(source_dir).as_posix()}"

        // For `SdlFunctionType`
        #include <Engine/SDL/Introspect/TSdlOwnerMethod.h>

        namespace ph
        {{

        const SdlFunctionType* PrimaryOwnerType::OwnerType::getSdlFunction()
        {{
            static_assert(std::is_base_of_v<::ph::SdlFunction, SdlFunctionType>,
                "PH_DEFINE_SDL_FUNCTION() must return a function derived from SdlFunction.");
            
            static const auto sdlFunction =
                []() -> SdlFunctionType
                {{
                    SdlFunctionType def;
                    TSdlFunctionDefiner<SdlFunctionType> definer(def);
                    internal_sdl_function_impl<SdlFunctionType>(definer);
                    return def;
                }};
            return &sdlFunction;
        }}

        }}// end namespace ph

        """

        results.append(result)

    return results

parser = argparse.ArgumentParser(description="PSDL Definition Tool")
args = parser.parse_args()

# Generate source according to definition
for name, section in config.get_all_projects(config.get_setup_config()):
    project_name = name.removeprefix("Project.")
    project_dir = Path(section['ProjectDirectory'])
    source_dir = project_dir / 'Source'
    generated_source_dir = project_dir / 'Generated' / 'SDL'

    generated_sources = []
    for dirpath, dirnames, filenames in source_dir.walk():
        for filename in filenames:
            if not filename.endswith('.h'):
                continue

            generated_sources.extend(generate_source_for(dirpath / filename, source_dir))
            
    if not generated_sources:
        continue

    print(f"[Project {name}] Generating source for {len(generated_sources)} definitions...")

    # Remove old source
    for item in generated_source_dir.iterdir():
        if not item.is_file() or item.suffix != '.cpp':
            raise ValueError(f"unexpected item found in {generated_source_dir}")
        
    # Write new source
    for generated_source in generated_sources:
        (generated_source_dir / f"def_{generated_source.owner_class_name}").with_suffix('.cpp').write_text(generated_source.sdl_impl)

    # TODO: generate source for ph_core.cpp and such
    
# TODO: indicate CMake reconfigure is required
