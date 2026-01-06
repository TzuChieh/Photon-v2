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
    DefineEnum = 4
    DeclareMeta = 100


class SourceFragment:
    def __init__(self):
        # Path to the header file that contained the macro. `None` if unapplicable or the macro
        # is not placed in header.
        self.macro_header = None
        
        self.macro_type = EMacro.Other

        # The main class type specified as macro argument. `None` if unapplicable.
        self.owner_class_type = None

        # TODO
        self.qualified_owner_class_type = None

        # The enum type specified as macro argument. `None` if unapplicable.
        self.enum_type = None

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

        self.project_name = ""

    @property
    @abstractmethod
    def macro_name(self) -> str:
        pass

    def generate_source(self, source_file: Path, source_dir: Path, arg_tokens: list[str]) -> SourceFragment:
        """
        Generate source code (fragment) for a single file.
        @return `None` if nothing is generated.
        """
        return None

    def post_generate_source(self, frags: list[SourceFragment]) -> SourceFragment:
        """
        Generate source code (fragment) after all source files are scanned. Have access to extra info.
        @return `None` if nothing is generated.
        """
        return None
    
    def set_project_name(self, name):
        self.project_name = name

    def _get_outer_scope(self, source_file: Path, arg_tokens: list[str]) -> str:
        """
        @return The scope as specified by `outerScope`. For `outerScope=` or `outerScope=void`,
        the scope is treated as no scope. If `outerScope` is not even present in arguments,
        filename is used as the scope.
        """
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


class ClassHandler(MacroHandler):
    @property
    def macro_name(self):
        return 'PH_DEFINE_SDL_CLASS'
    
    def generate_source(self, source_file, source_dir, arg_tokens):
        if source_file.suffix != '.h':
            raise ValueError(f"SDL class definition is only allowed in header (offending file: {source_file})")

        # For arguments, we need at least `CppOwnerType, classDef`
        if len(arg_tokens) < 2:
            raise ValueError(f"{self.macro_name}() requires at least 2 arguments, {len(arg_tokens)} were given")

        outer_scope = self._get_outer_scope(source_file, arg_tokens)
        outer_scope_expr = f"{outer_scope}::" if outer_scope else ""
        header_include_expr = source_file.relative_to(source_dir).as_posix()
        owner_class_type = arg_tokens[0]

        src = SourceFragment()
        src.macro_type = EMacro.DefineClass
        src.macro_header = source_file
        src.owner_class_type = owner_class_type
        src.sdl_impl = textwrap.dedent(
            f"""
            #include "{header_include_expr}"

            // For `SdlClassType`
            #include <Engine/SDL/Introspect/TSdlOwnerClass.h>

            namespace ph
            {{

            auto {outer_scope_expr}{owner_class_type}::getSdlClass()
            -> const TSdlOwnerClass<OwnerType>*
            {{
                static_assert(std::is_base_of_v<::ph::ISdlResource, OwnerType>,
                    "PH_DEFINE_SDL_CLASS() can only be defined for SDL resource.");

                using SdlClassType = TSdlOwnerClass<OwnerType>;
                static_assert(std::is_base_of_v<::ph::SdlClass, SdlClassType>,
                    "getSdlClass() must return a class derived from SdlClass.");
                
                static const auto sdlClass =
                    []() -> SdlClassType
                    {{
                        SdlClassType def;
                        TSdlClassDefiner<SdlClassType> definer(def);
                        internal_sdlClassDefinition<SdlClassType>(definer);
                        return def;
                    }}();
                return &sdlClass;
            }}

            }}// end namespace ph

            """)
        return src


class StructHandler(MacroHandler):
    @property
    def macro_name(self):
        return 'PH_DEFINE_SDL_STRUCT'
    
    def generate_source(self, source_file, source_dir, arg_tokens):
        if source_file.suffix != '.h':
            raise ValueError(f"SDL struct definition is only allowed in header (offending file: {source_file})")

        # For arguments, we need at least `CppOwnerType, structDef`
        if len(arg_tokens) < 2:
            raise ValueError(f"{self.macro_name}() requires at least 2 arguments, {len(arg_tokens)} were given")

        outer_scope = self._get_outer_scope(source_file, arg_tokens)
        outer_scope_expr = f"{outer_scope}::" if outer_scope else ""
        header_include_expr = source_file.relative_to(source_dir).as_posix()
        owner_class_type = arg_tokens[0]

        src = SourceFragment()
        src.macro_type = EMacro.DefineStruct
        src.macro_header = source_file
        src.owner_class_type = owner_class_type
        src.sdl_impl = textwrap.dedent(
            f"""
            #include "{header_include_expr}"

            // For `SdlStructType`
            #include <Engine/SDL/Introspect/TSdlOwnerStruct.h>

            namespace ph
            {{

            auto {outer_scope_expr}{owner_class_type}::getSdlStruct()
            -> const TSdlOwnerStruct<OwnerType>*
            {{
                using SdlStructType = TSdlOwnerStruct<OwnerType>;
                static_assert(std::is_base_of_v<::ph::SdlStruct, SdlStructType>,
                    "getSdlStruct() must return a struct derived from SdlStruct.");
                
                static const auto sdlStruct =
                    []() -> SdlStructType
                    {{
                        SdlStructType def;
                        TSdlStructDefiner<SdlStructType> definer(def);
                        internal_sdlStructDefinition<SdlStructType>(definer);
                        return def;
                    }}();
                return &sdlStruct;
            }}

            }}// end namespace ph

            """)
        return src


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

        outer_scope = self._get_outer_scope(source_file, arg_tokens)
        outer_scope_expr = f"{outer_scope}::" if outer_scope else ""
        header_include_expr = source_file.relative_to(source_dir).as_posix()
        owner_class_type = arg_tokens[0]

        src = SourceFragment()
        src.macro_type = EMacro.DefineFunction
        src.macro_header = source_file
        src.owner_class_type = owner_class_type
        src.sdl_impl = textwrap.dedent(
            f"""
            #include "{header_include_expr}"

            // For `SdlFunctionType`
            #include <Engine/SDL/Introspect/TSdlOwnerMethod.h>

            namespace ph
            {{

            auto {outer_scope_expr}{owner_class_type}::getSdlFunction()
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
                        internal_sdlFunctionDefinition<SdlFunctionType>(definer);
                        return def;
                    }}();
                return &sdlFunction;
            }}

            }}// end namespace ph

            """)
        return src


class EnumHandler(MacroHandler):
    @property
    def macro_name(self):
        return 'PH_DEFINE_SDL_ENUM'
    
    def generate_source(self, source_file, source_dir, arg_tokens):
        if source_file.suffix != '.h':
            raise ValueError(f"SDL enum definition is only allowed in header (offending file: {source_file})")

        # For arguments, we need at least `enumType`, `enumDef`
        if len(arg_tokens) < 2:
            raise ValueError(f"{self.macro_name}() requires at least 2 arguments, {len(arg_tokens)} were given")

        outer_scope = self._get_value('outerScope', arg_tokens)
        if outer_scope is not None:
            print(f"warning: for {self.macro_name}(), outerScope is ignored (file: {source_file})")

        header_include_expr = source_file.relative_to(source_dir).as_posix()
        enum_type = arg_tokens[0]

        src = SourceFragment()
        src.macro_type = EMacro.DefineEnum
        src.macro_header = source_file
        src.enum_type = enum_type
        src.sdl_impl = textwrap.dedent(
            f"""
            #include "{header_include_expr}"

            // For `SdlEnumType`
            #include <Engine/SDL/Introspect/TSdlGeneralEnum.h>

            namespace ph
            {{

            auto TSdlEnum<std::remove_cv_t<typename {enum_type}>>::getSdlEnum()
            -> const SdlEnumType*
            {{
                static_assert(std::is_base_of_v<::ph::SdlEnum, SdlEnumType>,
                    "getSdlEnum() must return an enum derived from SdlEnum.");
                
                static const auto sdlEnum =
                    []() -> SdlEnumType
                    {{
                        SdlEnumType def;
                        TSdlEnumDefiner<SdlEnumType> definer(def);
                        internal_sdlEnumDefinition<SdlEnumType>(definer);
                        return def;
                    }}();
                return &sdlEnum;
            }}

            }}// end namespace ph

            """)
        return src


class MetaGetterForAllSdlClassesHandler(MacroHandler):
    def __init__(self):
        super().__init__()

        self.generate_source_args = []

    @property
    def macro_name(self):
        return 'PH_DECLARE_GETTER_FOR_ALL_SDL_CLASSES'
    
    def generate_source(self, source_file, source_dir, arg_tokens):
        # This macro can be used anywhere; store required info for post generation later
        self.generate_source_args.append((source_file, source_dir, arg_tokens))
        return None
    
    def post_generate_source(self, frags):
        src = SourceFragment()
        src.macro_type = EMacro.DeclareMeta

        owner_class_defs = [f for f in frags if f.macro_type == EMacro.DefineClass]
        for source_file, source_dir, arg_tokens in self.generate_source_args:
            # For arguments, we need at least `getterFuncName`
            if len(arg_tokens) < 1:
                raise ValueError(f"{self.macro_name}() requires at least 1 arguments, {len(arg_tokens)} were given")

            getter_name = arg_tokens[0]
            register_func_name = f"register_sdl_class_for_{getter_name}"

            sdl_classes = "\n"
            sdl_class_includes = "\n"
            for owner_class_def in owner_class_defs:
                sdl_classes += f"{register_func_name}<{owner_class_def.owner_class_type}>(),\n"

                header_include_expr = owner_class_def.macro_header.relative_to(source_dir).as_posix()
                sdl_class_includes += f"#include \"{header_include_expr}\"\n"
            
            designer_obj_register = "\n"
            if self.project_name == 'EditorLib':
                designer_obj_register = textwrap.dedent(
                    f"""
                    // Register for dynamic designer object creation
                    if constexpr(CDerived<SdlClassType, DesignerObject>)
                    {{
                        DesignerScene::registerObjectType<SdlClassType>();
                    }}
                    """)

            # Just for aesthetics of generated code
            sdl_classes = textwrap.indent(sdl_classes, "                        ")
            sdl_class_includes = textwrap.indent(sdl_class_includes, "                ")
            designer_obj_register = textwrap.indent(designer_obj_register, "                    ")

            # Supports namespace scope only--we are not including function declaration so
            # specifying scope on function name will cause "namespace has no such member" error
            outer_scope = self._get_outer_scope(source_file, arg_tokens)
            outer_scope_expr = f"::{outer_scope}" if outer_scope else ""

            src.sdl_impl += textwrap.dedent(
                f"""
                {sdl_class_includes}

                #include <Engine/Utility/traits.h>

                #include <vector>

                namespace ph{outer_scope_expr}
                {{
                
                template<typename SdlClassType>
                inline const SdlClass* {register_func_name}()
                {{
                    const SdlClass* const clazz = SdlClassType::getSdlClass();

                    {designer_obj_register}

                    return clazz;
                }}

                std::vector<const SdlClass*> {getter_name}()
                {{
                    return
                    {{
                        {sdl_classes}
                    }};
                }}

                }}// end namespace ph{outer_scope_expr}

                """)
        return src if src.sdl_impl else None
    

class MetaGetterForAllSdlEnumsHandler(MacroHandler):
    def __init__(self):
        super().__init__()

        self.generate_source_args = []

    @property
    def macro_name(self):
        return 'PH_DECLARE_GETTER_FOR_ALL_SDL_ENUMS'
    
    def generate_source(self, source_file, source_dir, arg_tokens):
        # This macro can be used anywhere; store required info for post generation later
        self.generate_source_args.append((source_file, source_dir, arg_tokens))
        return None
    
    def post_generate_source(self, frags):
        src = SourceFragment()
        src.macro_type = EMacro.DeclareMeta

        enum_defs = [f for f in frags if f.macro_type == EMacro.DefineEnum]
        for source_file, source_dir, arg_tokens in self.generate_source_args:
            # For arguments, we need at least `getterFuncName`
            if len(arg_tokens) < 1:
                raise ValueError(f"{self.macro_name}() requires at least 1 arguments, {len(arg_tokens)} were given")

            sdl_enums = "\n"
            sdl_enum_includes = "\n"
            for enum_def in enum_defs:
                sdl_enums += f"TSdlEnum<typename {enum_def.enum_type}>::getSdlEnum(),\n"

                header_include_expr = enum_def.macro_header.relative_to(source_dir).as_posix()
                sdl_enum_includes += f"#include \"{header_include_expr}\"\n"
            
            # Just for aesthetics of generated code
            sdl_enums = textwrap.indent(sdl_enums, "                        ")
            sdl_enum_includes = textwrap.indent(sdl_enum_includes, "                ")

            getter_name = arg_tokens[0]

            # Supports namespace scope only--we are not including function declaration so
            # specifying scope on function name will cause "namespace has no such member" error
            outer_scope = self._get_outer_scope(source_file, arg_tokens)
            outer_scope_expr = f"::{outer_scope}" if outer_scope else ""

            src.sdl_impl += textwrap.dedent(
                f"""
                {sdl_enum_includes}

                #include <vector>

                namespace ph{outer_scope_expr}
                {{

                std::vector<const SdlEnum*> {getter_name}()
                {{
                    return
                    {{
                        {sdl_enums}
                    }};
                }}

                }}// end namespace ph{outer_scope_expr}

                """)
        return src if src.sdl_impl else None


def _generate_source_for(source_file: Path, source_dir: Path, handlers: list[MacroHandler]):
    """
    Generate source code for a single file.
    @return A single compilation unit (`CompilationUnit`). `None` if nothing is generated.
    """
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
            frag = handler.generate_source(source_file, source_dir, tokens)
            if frag:
                sdl_frags.append(frag)

            # Handlers are unique, skip the rests if we handled one
            break

    if not sdl_frags:
        return None

    unit = CompilationUnit()
    unit.unique_name = source_file.relative_to(source_dir).with_suffix('').as_posix().replace('/', '_')
    unit.sdl_frags = sdl_frags
    unit.version_id = uuid.uuid4()
    return unit

def _post_generate_source_for(units: list[CompilationUnit], handlers: list[MacroHandler]):
    """
    Post generate source code for each compilation unit.
    @param[in,out] units Compilation units to process. They can also be modified during the process.
    @return A list if compilation units (`list[CompilationUnit]`) generated additionally.
    """
    frags = [f for unit in units for f in unit.sdl_frags]

    post_units = []
    for handler in handlers:
        post_frag = handler.post_generate_source(frags)
        if post_frag:
            post_unit = CompilationUnit()
            post_unit.unique_name = f"post_{handler.macro_name}"
            post_unit.sdl_frags = [post_frag]
            post_unit.version_id = uuid.uuid4()
            post_units.append(post_unit)

    return post_units


def _init_macro_handlers(project_name) -> list[MacroHandler]:
    handlers = [
        ClassHandler(),
        StructHandler(),
        FunctionHandler(),
        EnumHandler(),
        MetaGetterForAllSdlClassesHandler(),
        MetaGetterForAllSdlEnumsHandler(),
        ]
    
    for handler in handlers:
        handler.set_project_name(project_name)
    
    return handlers


def generate(setup_config: configparser.ConfigParser):
    # Generate for each project
    for name, section in config.get_all_projects(setup_config):
        project_name = name.removeprefix("Project.")
        handlers = _init_macro_handlers(project_name)
        project_dir = Path(section['ProjectDirectory'])
        source_dir = project_dir / 'Source'
        generated_source_dir = project_dir / 'Generated' / 'SDL'
        log_name = f"[Project {project_name}]"

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

        units.extend(_post_generate_source_for(units, handlers))

        # Remove old source
        if generated_source_dir.is_dir():
            for item in generated_source_dir.iterdir():
                if not item.is_file() or item.suffix != '.cpp':
                    raise ValueError(f"unexpected item found in {generated_source_dir}")
                
                item.unlink()

        if not units:
            continue

        print(f"{log_name} Writing {len(units)} compilation unit(s)...")

        generated_source_dir.mkdir(parents=True, exist_ok=True)
            
        # Write new source
        for unit in units:
            (generated_source_dir / f"def_{unit.unique_name}").with_suffix('.cpp').write_text(unit.generate_source_code())

        # TODO: generate source for ph_core.cpp and such
        
    # TODO: indicate CMake reconfigure is required

    print(f"All definitions generated.")
