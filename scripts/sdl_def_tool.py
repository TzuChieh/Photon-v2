from utility import config

import configparser
import textwrap
import time
from pathlib import Path
from abc import ABC, abstractmethod


class GeneratedSource:
    def __init__(self):
        self.unique_name = ""
        self.sdl_impl = ""


class MacroHandler(ABC):
    def __init__(self):
        super().__init__()

    @property
    @abstractmethod
    def macro_name(self) -> str:
        pass

    @abstractmethod
    def generate_source(self, source_file: Path, source_dir: Path, arg_tokens: list[str]) -> str:
        pass


class SdlFunctionHandler(MacroHandler):
    @property
    def macro_name(self):
        return 'PH_DEFINE_SDL_FUNCTION_'
    
    def generate_source(self, source_file, source_dir, arg_tokens):
        primary_owner_class_name = source_file.stem
        header_include_expr = source_file.relative_to(source_dir).as_posix()

        # For arguments, we need at least `CppOwnerType, funcDef`
        if len(arg_tokens) < 2:
            raise ValueError(f"{self.macro_name}() requires at least 2 arguments, {len(arg_tokens)} were given")

        owner_class_name = arg_tokens[0].strip()

        return textwrap.dedent(
            f"""
            #include "{header_include_expr}"

            // For `SdlFunctionType`
            #include <Engine/SDL/Introspect/TSdlOwnerMethod.h>

            namespace ph
            {{

            auto {primary_owner_class_name}::{owner_class_name}::getSdlFunction()
            -> const TSdlOwnerMethod<OwnerType, std::remove_cvref_t<TCallableTraits<OwnerType>::ArgTypeAt<0>>>*
            {{
                using SdlFunctionType = TSdlOwnerMethod<OwnerType, std::remove_cvref_t<TCallableTraits<OwnerType>::ArgTypeAt<0>>>;
                static_assert(std::is_base_of_v<::ph::SdlFunction, SdlFunctionType>,
                    "getSdlFunction() must return a function derived from SdlFunction.");
                
                static const auto sdlFunction =
                    []() -> SdlFunctionType
                    {{
                        SdlFunctionType def;
                        TSdlFunctionDefiner<SdlFunctionType> definer(def);
                        internal_sdl_definition_impl<SdlFunctionType>(definer);
                        return def;
                    }}();
                return &sdlFunction;
            }}

            }}// end namespace ph

            """)


def _generate_source_for(source_file: Path, source_dir: Path, handlers: list[MacroHandler]):
    sdl_impl = ""
    for line in source_file.read_text(encoding='utf-8').splitlines():
        line = line.strip()
        for handler in handlers:
            macro_prefix = handler.macro_name + '('
            if not line.startswith(macro_prefix):
                continue

            tokens = line[len(macro_prefix):].split(',')
            sdl_impl += handler.generate_source(source_file, source_dir, tokens)

            # Handlers are unique, skip the rests if we handled one
            break

    if not sdl_impl:
        return None

    result = GeneratedSource()
    result.unique_name = source_file.relative_to(source_dir).as_posix().replace('/', '_')
    result.sdl_impl = f"// !!! GENERATED CODE, DO NOT MODIFY !!! ID: {time.time_ns()}\n{sdl_impl}"
    return result

def generate(setup_config: configparser.ConfigParser):
    handlers = [
        SdlFunctionHandler()
        ]

    # Generate for each project
    for name, section in config.get_all_projects(setup_config):
        project_name = name.removeprefix("Project.")
        project_dir = Path(section['ProjectDirectory'])
        source_dir = project_dir / 'Source'
        generated_source_dir = project_dir / 'Generated' / 'SDL'

        # Generate for each header file
        generated_sources = []
        for dirpath, dirnames, filenames in source_dir.walk():
            for filename in filenames:
                if not filename.endswith(('.h', '.cpp', '.ipp', '.tpp')):
                    continue

                generated_source = _generate_source_for(dirpath / filename, source_dir, handlers)
                if generated_source is not None:
                    generated_sources.append(generated_source)
                
        if not generated_sources:
            continue

        print(f"[Project {project_name}] Generating source for {len(generated_sources)} definitions...")

        generated_source_dir.mkdir(parents=True, exist_ok=True)

        # Remove old source
        for item in generated_source_dir.iterdir():
            if not item.is_file() or item.suffix != '.cpp':
                raise ValueError(f"unexpected item found in {generated_source_dir}")
            
            item.unlink()
            
        # Write new source
        for generated_source in generated_sources:
            (generated_source_dir / f"def_{generated_source.unique_name}").with_suffix('.cpp').write_text(generated_source.sdl_impl)

        # TODO: remove extra indent
        # TODO: CMake script to include source for compile
        # TODO: generate source for ph_core.cpp and such
        # TODO: outerType=? option for test
        
    # TODO: indicate CMake reconfigure is required

    print(f"All definitions generated.")
