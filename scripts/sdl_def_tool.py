from utility import config

import configparser
import textwrap
import uuid
import re
from pathlib import Path
from abc import ABC, abstractmethod
from enum import Enum


class EMacro(Enum):
    """
    Type and purpose of the macro.
    """
    Other = 0
    DefineClass = 1
    DefineStruct = 2
    DefineFunction = 3
    DeclareMeta = 4


class SourceFragment:
    def __init__(self):
        # Path to the header file that contained the macro. `None` if unapplicable or the macro
        # is not placed in header.
        self.macro_header = None
        
        self.macro_type = EMacro.Other
        self.owner_class = ""
        self.sdl_impl = ""


class CompilationUnit:
    """
    Source code that can be compiled as a single file.
    """
    def __init__(self):
        self.unique_name = ""

        # A stringifiable value for marking versions of generated source
        self.version_id = 0

        self.sdl_frags = []

    def generate_source_code(self):
        source = f"// !!! GENERATED CODE, DO NOT MODIFY !!! ID: {self.version_id}\n"
        for sdl_frag in self.sdl_frags:
            source += sdl_frag.sdl_impl
        return source


class MacroHandler(ABC):
    def __init__(self):
        super().__init__()

    @property
    @abstractmethod
    def macro_name(self) -> str:
        pass

    @abstractmethod
    def generate_source(self, source_file: Path, source_dir: Path, arg_tokens: list[str]) -> SourceFragment:
        pass

    def _get_outer_scope_expr(self, source_file: Path, arg_tokens: list[str]) -> str:
        outer_scope = self._get_outer_scope(source_file, arg_tokens)
        return "" if not outer_scope else f"{outer_scope}::"

    def _get_outer_scope(self, source_file: Path, arg_tokens: list[str]) -> str:
        outer_scope = self._get_value('outerScope', arg_tokens)
        if outer_scope is None:
            outer_scope = source_file.stem
        elif not outer_scope or outer_scope == 'void':
            outer_scope = ""
        return outer_scope

    def _get_value(self, arg_name, arg_tokens: list[str]) -> str | None:
        """
        @return Extracted value of the specified named argument. `None` if the argument is not found.
        """
        for token in arg_tokens:
            if token.startswith(arg_name):
                token = token[len(arg_name):].lstrip()
                if token.startswith('='):
                    return token[1:].lstrip()
                else:
                    return ""
        return None


class FunctionHandler(MacroHandler):
    @property
    def macro_name(self):
        return 'PH_DEFINE_SDL_FUNCTION'
    
    def generate_source(self, source_file, source_dir, arg_tokens):
        if source_file.suffix != '.h':
            raise ValueError(f"SDL function definition is only allowed in header (offending file: {source_file})")

        # For arguments, we need at least `CppOwnerType, funcDef`
        if len(arg_tokens) < 2:
            raise ValueError(f"{self.macro_name}() requires at least 2 arguments, {len(arg_tokens)} were given")

        outer_scope_expr = self._get_outer_scope_expr(source_file, arg_tokens)
        header_include_expr = source_file.relative_to(source_dir).as_posix()
        owner_class_name = arg_tokens[0]

        src = SourceFragment()
        src.macro_type = EMacro.DefineFunction
        src.macro_header = source_file
        src.owner_class = owner_class_name
        src.sdl_impl = textwrap.dedent(
            f"""
            #include "{header_include_expr}"

            // For `SdlFunctionType`
            #include <Engine/SDL/Introspect/TSdlOwnerMethod.h>

            namespace ph
            {{

            auto {outer_scope_expr}{owner_class_name}::getSdlFunction()
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
        return src


# class MetaGetterForAllSdlClassesHandler(MacroHandler):
#     def __init__(self, ):
#         super().__init__()



#     @property
#     def macro_name(self):
#         return 'PH_DECLARE_GETTER_FOR_ALL_SDL_CLASSES'
    
#     def generate_source(self, source_file, source_dir, arg_tokens):
#         primary_owner_class_name = source_file.stem
#         header_include_expr = source_file.relative_to(source_dir).as_posix()

#         # For arguments, we need at least `getterFuncName`
#         if len(arg_tokens) < 1:
#             raise ValueError(f"{self.macro_name}() requires at least 1 argument, {len(arg_tokens)} were given")

#         getter_func_name = arg_tokens[0].strip()

#         return textwrap.dedent(
#             f"""
#             #include "{header_include_expr}"

#             // For `SdlFunctionType`
#             #include <Engine/SDL/Introspect/TSdlOwnerMethod.h>

#             namespace ph
#             {{

#             auto {primary_owner_class_name}::{owner_class_name}::getSdlFunction()
#             -> const TSdlOwnerMethod<OwnerType, std::remove_cvref_t<TCallableTraits<OwnerType>::ArgTypeAt<0>>>*
#             {{
#                 using SdlFunctionType = TSdlOwnerMethod<OwnerType, std::remove_cvref_t<TCallableTraits<OwnerType>::ArgTypeAt<0>>>;
#                 static_assert(std::is_base_of_v<::ph::SdlFunction, SdlFunctionType>,
#                     "getSdlFunction() must return a function derived from SdlFunction.");
                
#                 static const auto sdlFunction =
#                     []() -> SdlFunctionType
#                     {{
#                         SdlFunctionType def;
#                         TSdlFunctionDefiner<SdlFunctionType> definer(def);
#                         internal_sdl_definition_impl<SdlFunctionType>(definer);
#                         return def;
#                     }}();
#                 return &sdlFunction;
#             }}

#             }}// end namespace ph

#             """)
    

def _generate_source_for(source_file: Path, source_dir: Path, handlers: list[MacroHandler]):
    sdl_frags = []
    for line in source_file.read_text(encoding='utf-8').splitlines():
        line = line.lstrip()
        for handler in handlers:
            # This rejects most cases; we want to scan as fast as possible
            if not line.startswith(handler.macro_name):
                continue

            # If a macro name is recognized, extract the args within parentheses
            line_matches = re.search(r'\((.*?)\)', line)
            if line_matches is None:
                continue

            str_within_parentheses = line_matches.group(1)
            tokens = [token.strip() for token in str_within_parentheses.split(',')]
            sdl_frags.append(handler.generate_source(source_file, source_dir, tokens))

            # Handlers are unique, skip the rests if we handled one
            break

    if not sdl_frags:
        return None

    unit = CompilationUnit()
    unit.unique_name = source_file.relative_to(source_dir).with_suffix('').as_posix().replace('/', '_')
    unit.sdl_frags = sdl_frags
    unit.version_id = uuid.uuid4()
    return unit

def generate(setup_config: configparser.ConfigParser):
    handlers = [
        FunctionHandler()
        ]

    # Generate for each project
    for name, section in config.get_all_projects(setup_config):
        project_dir = Path(section['ProjectDirectory'])
        source_dir = project_dir / 'Source'
        generated_source_dir = project_dir / 'Generated' / 'SDL'
        log_name = f"[Project {name.removeprefix("Project.")}]"

        # Generate for each header file
        units = []
        num_source_files = 0
        for dirpath, dirnames, filenames in source_dir.walk():
            for filename in filenames:
                num_source_files += 1
                if not filename.endswith(('.h', '.cpp', '.ipp', '.tpp')):
                    continue

                unit = _generate_source_for(dirpath / filename, source_dir, handlers)
                if unit is not None:
                    units.append(unit)
                
        print(f"{log_name} Processed {num_source_files} source files")

        if not units:
            continue

        print(f"{log_name} Writing source for {len(units)} definitions...")

        generated_source_dir.mkdir(parents=True, exist_ok=True)

        # Remove old source
        for item in generated_source_dir.iterdir():
            if not item.is_file() or item.suffix != '.cpp':
                raise ValueError(f"unexpected item found in {generated_source_dir}")
            
            item.unlink()
            
        # Write new source
        for unit in units:
            (generated_source_dir / f"def_{unit.unique_name}").with_suffix('.cpp').write_text(unit.generate_source_code())

        # TODO: remove extra indent
        # TODO: CMake script to include source for compile
        # TODO: generate source for ph_core.cpp and such
        # TODO: outerType=? option for test
        
    # TODO: indicate CMake reconfigure is required

    print(f"All definitions generated.")
