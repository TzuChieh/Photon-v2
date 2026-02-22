from utility import config

import configparser
import textwrap
import uuid
import re
from pathlib import Path
from abc import ABC, abstractmethod
from enum import Enum, auto
from collections import OrderedDict

# For Python < 3.10, we cannot use `|`
from typing import Union


def _to_std_map_literal(user_specs: OrderedDict[str, list[str]]):
    pair_literals = ""
    for key, values in user_specs.items():
        quoted_key = f"\"{key}\""
        quoted_values = ", ".join(f"\"{value}\"" for value in values)
        pair_literals += f"{{{quoted_key}, {{{quoted_values}}}}}, "
    return f"{{{pair_literals}}}"


class EMacro(Enum):
    """
    Type and purpose of the macro.
    """
    Other = 0
    DefineClass = auto()
    DefineStruct = auto()
    DefineMethod = auto()
    DefineStaticMethod = auto()
    DefineEnum = auto()
    DeclareMeta = 100


class MacroExtraction:
    handler: Union["MacroHandler"]
    project_name: str
    source_file: Union[Path, None]
    source_dir: Union[Path, None]
    user_specs: OrderedDict[str, list[str]]
    impl = Union["MacroImplementation", None]

    def __init__(self):
        # The macro handler that should process the macro
        self.handler = None

        # Name of the project the macro was extracted from
        self.project_name = ""

        # Path to the source file that contains the macro
        self.source_file = None

        # The source file's base source directory
        self.source_dir = None

        # User specified arguments to the macro
        self.user_specs = OrderedDict()

        # If not `None`, this is the implementation generated from this macro
        self.impl = None

    @property
    def source_include_expr(self, quote_char='"') -> str:
        """
        Helper for getting a C++ include expression of the source file (the part after #include).
        """
        include_path = self.source_file.relative_to(self.source_dir).as_posix()
        return f"#include {quote_char}{include_path}{quote_char}"
    
    @property
    def outer_scope(self) -> str:
        """
        :returns: The scope as specified by `outerScope`. For `outerScope=` or `outerScope=void`,
        the scope is treated as no scope. If `outerScope` is not even present in arguments,
        filename is used as the scope.
        """
        # Default to filename
        if 'outerScope' not in self.user_specs:
            outer_scope = self.source_file.stem
        # Explicitly stated to have no scope
        elif not self.user_specs['outerScope'] or self.user_specs['outerScope'][0] == 'void':
            outer_scope = ""
        else:
            outer_scope = self.user_specs['outerScope'][0]
        return outer_scope
    
    def user_spec_at(self, arg_idx) -> str:
        """
        Helper for accessing a positional user argument (key without mapped values).
        """
        # Arguments are short typically, so we simply make a list and get i-th element
        return list(self.user_specs.keys())[arg_idx]


class MacroImplementation:
    macro_header: Union[Path, None]
    macro_type: EMacro
    owner_class_type: str
    enum_type = str
    source = str

    def __init__(self):
        # Path to the header file that contained the macro. `None` if unapplicable or the macro
        # is not placed in header.
        self.macro_header = None
        
        self.macro_type = EMacro.Other

        # The main class type specified as macro argument. Empty if unapplicable.
        self.owner_class_type = ""

        # TODO
        self.qualified_owner_class_type = ""

        # The enum type specified as macro argument. Empty if unapplicable.
        self.enum_type = ""

        # Generated source code
        self.source = ""


class CompilationUnit:
    """
    Source code that can be compiled as a single file.
    """
    impls: list[MacroImplementation]

    def __init__(self):
        self.unique_name = ""

        # A stringifiable value for marking versions of generated source
        self.version_id = 0

        self.impls = []

    def generate_source_code(self):
        source = f"// !!! GENERATED CODE, DO NOT MODIFY !!! ID: {self.version_id}\n"
        for impl in self.impls:
            source += impl.source
        return source


class MacroHandler(ABC):
    def __init__(self):
        super().__init__()

    @property
    @abstractmethod
    def macro_name(self) -> str:
        pass

    def generate_source(self, extr: MacroExtraction) -> Union[MacroImplementation, None]:
        """
        Generate source code for a single macro. Called once for each macro usage.
        
        :returns: `None` if nothing is generated.
        """
        return None

    def post_generate_source(self, extrs: list[MacroExtraction]) -> Union[MacroImplementation, None]:
        """
        Generate source code (fragment) after all source files are scanned. Have access to extra info.

        :param extrs: All extractions in the same project. MacroExtraction.impl will contain the
        generated source returned by `generate_source()`.
        :returns: `None` if nothing is generated.
        """
        return None
    
    def post_generate_source_for_all_projects(self, current_project: str, project_name_to_extrs: dict[str, list[MacroExtraction]]) -> Union[MacroImplementation, None]:
        """
        Called once for each project. Similar to `post_generate_source()`, but with extractions from all projects.
        
        :param current_project: Current project's name.
        """
        return None


class ClassHandler(MacroHandler):
    @property
    def macro_name(self):
        return 'PH_DEFINE_SDL_CLASS'
    
    def generate_source(self, extr):
        if extr.source_file.suffix != '.h':
            raise ValueError(f"SDL class definition is only allowed in header (offending file: {extr.source_file})")

        # For arguments, we need at least `CppOwnerType, classDef`
        if len(extr.user_specs) < 2:
            raise ValueError(f"{self.macro_name}() requires at least 2 arguments, {len(extr.user_specs)} were given")

        outer_scope_expr = f"{extr.outer_scope}::" if extr.outer_scope else ""
        owner_class_type = extr.user_spec_at(0)

        src = MacroImplementation()
        src.macro_type = EMacro.DefineClass
        src.macro_header = extr.source_file
        src.owner_class_type = owner_class_type
        src.source = textwrap.dedent(
            f"""
            {extr.source_include_expr}

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
                        def.userSpec(SdlUserSpec({_to_std_map_literal(extr.user_specs)}));
                        
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
    
    def generate_source(self, extr):
        if extr.source_file.suffix != '.h':
            raise ValueError(f"SDL struct definition is only allowed in header (offending file: {extr.source_file})")

        # For arguments, we need at least `CppOwnerType, structDef`
        if len(extr.user_specs) < 2:
            raise ValueError(f"{self.macro_name}() requires at least 2 arguments, {len(extr.user_specs)} were given")

        outer_scope_expr = f"{extr.outer_scope}::" if extr.outer_scope else ""
        owner_class_type = extr.user_spec_at(0)

        src = MacroImplementation()
        src.macro_type = EMacro.DefineStruct
        src.macro_header = extr.source_file
        src.owner_class_type = owner_class_type
        src.source = textwrap.dedent(
            f"""
            {extr.source_include_expr}

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
                        def.userSpec(SdlUserSpec({_to_std_map_literal(extr.user_specs)}));
                        
                        TSdlStructDefiner<SdlStructType> definer(def);
                        internal_sdlStructDefinition<SdlStructType>(definer);
                        return def;
                    }}();
                return &sdlStruct;
            }}

            }}// end namespace ph

            """)
        return src


class MethodHandler(MacroHandler):
    @property
    def macro_name(self):
        return 'PH_DEFINE_SDL_METHOD'
    
    def generate_source(self, extr):
        if extr.source_file.suffix != '.h':
            raise ValueError(f"SDL method definition is only allowed in header (offending file: {extr.source_file})")

        # For arguments, we need at least `CppOwnerType, funcDef`
        if len(extr.user_specs) < 2:
            raise ValueError(f"{self.macro_name}() requires at least 2 arguments, {len(extr.user_specs)} were given")

        outer_scope_expr = f"{extr.outer_scope}::" if extr.outer_scope else ""
        owner_class_type = extr.user_spec_at(0)

        src = MacroImplementation()
        src.macro_type = EMacro.DefineMethod
        src.macro_header = extr.source_file
        src.owner_class_type = owner_class_type
        src.source = textwrap.dedent(
            f"""
            {extr.source_include_expr}

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
                        def.userSpec(SdlUserSpec({_to_std_map_literal(extr.user_specs)}));
                        
                        TSdlFunctionDefiner<SdlFunctionType> definer(def);
                        internal_sdlFunctionDefinition<SdlFunctionType>(definer);
                        return def;
                    }}();
                return &sdlFunction;
            }}

            }}// end namespace ph

            """)
        return src


class StaticMethodHandler(MacroHandler):
    @property
    def macro_name(self):
        return 'PH_DEFINE_SDL_STATIC_METHOD'
    
    def generate_source(self, extr):
        if extr.source_file.suffix != '.h':
            raise ValueError(f"SDL static method definition is only allowed in header (offending file: {extr.source_file})")

        # For arguments, we need at least `CppOwnerType, funcDef`
        if len(extr.user_specs) < 2:
            raise ValueError(f"{self.macro_name}() requires at least 2 arguments, {len(extr.user_specs)} were given")

        outer_scope_expr = f"{extr.outer_scope}::" if extr.outer_scope else ""
        owner_class_type = extr.user_spec_at(0)

        src = MacroImplementation()
        src.macro_type = EMacro.DefineStaticMethod
        src.macro_header = extr.source_file
        src.owner_class_type = owner_class_type
        src.source = textwrap.dedent(
            f"""
            {extr.source_include_expr}

            // For `SdlFunctionType`
            #include <Engine/SDL/Introspect/TSdlOwnerStaticMethod.h>

            namespace ph
            {{

            auto {outer_scope_expr}{owner_class_type}::getSdlFunction()
            -> const TSdlOwnerStaticMethod<OwnerType>*
            {{
                using SdlFunctionType = TSdlOwnerStaticMethod<OwnerType>;
                static_assert(std::is_base_of_v<::ph::SdlFunction, SdlFunctionType>,
                    "getSdlFunction() must return a function derived from SdlFunction.");
                
                static const auto sdlFunction =
                    []() -> SdlFunctionType
                    {{
                        SdlFunctionType def;
                        def.userSpec(SdlUserSpec({_to_std_map_literal(extr.user_specs)}));
                        
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
    
    def generate_source(self, extr):
        if extr.source_file.suffix != '.h':
            raise ValueError(f"SDL enum definition is only allowed in header (offending file: {extr.source_file})")

        # For arguments, we need at least `enumType`, `enumDef`
        if len(extr.user_specs) < 2:
            raise ValueError(f"{self.macro_name}() requires at least 2 arguments, {len(extr.user_specs)} were given")

        if 'outerScope' in extr.user_specs:
            print(f"warning: for {self.macro_name}(), outerScope is ignored (file: {extr.source_file})")

        enum_type = extr.user_spec_at(0)

        src = MacroImplementation()
        src.macro_type = EMacro.DefineEnum
        src.macro_header = extr.source_file
        src.enum_type = enum_type
        src.source = textwrap.dedent(
            f"""
            {extr.source_include_expr}

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
                        def.userSpec(SdlUserSpec({_to_std_map_literal(extr.user_specs)}));
                        
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

        self.cached_extrs = []

    @property
    def macro_name(self):
        return 'PH_DECLARE_GETTER_FOR_ALL_SDL_CLASSES'
    
    def generate_source(self, extr):
        # For arguments, we need at least `getterFuncName`
        if len(extr.user_specs) < 1:
            raise ValueError(f"{self.macro_name}() requires at least 1 arguments, {len(extr.user_specs)} were given")

        # This macro can be used anywhere; store required info for post generation later
        self.cached_extrs.append(extr)
        return None
    
    def post_generate_source(self, extrs):
        src = MacroImplementation()
        src.macro_type = EMacro.DeclareMeta

        class_defs = [e for e in extrs if e.impl is not None and e.impl.macro_type == EMacro.DefineClass]
        for extr in self.cached_extrs:
            getter_name = extr.user_spec_at(0)
            register_func_name = f"register_sdl_class_for_{getter_name}"

            sdl_classes = "\n"
            sdl_class_includes = "\n"
            for class_def in class_defs:
                sdl_classes += f"{register_func_name}<{class_def.impl.owner_class_type}>(),\n"
                sdl_class_includes += f"{class_def.source_include_expr}\n"
            
            designer_obj_register = "\n"
            if extr.project_name == 'EditorLib':
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
            # specifying scope (such as class scope) on function name will cause "namespace has no such member" error
            outer_scope_expr = f"::{extr.outer_scope}" if extr.outer_scope else ""

            src.source += textwrap.dedent(
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
            
        self.cached_extrs = []

        return src if src.source else None
    

class MetaGetterForAllSdlEnumsHandler(MacroHandler):
    def __init__(self):
        super().__init__()

        self.cached_extrs = []

    @property
    def macro_name(self):
        return 'PH_DECLARE_GETTER_FOR_ALL_SDL_ENUMS'

    def generate_source(self, extr):
        # For arguments, we need at least `getterFuncName`
        if len(extr.user_specs) < 1:
            raise ValueError(f"{self.macro_name}() requires at least 1 arguments, {len(extr.user_specs)} were given")

        # This macro can be used anywhere; store required info for post generation later
        self.cached_extrs.append(extr)
        return None
    
    def post_generate_source(self, extrs):
        src = MacroImplementation()
        src.macro_type = EMacro.DeclareMeta

        enum_defs = [e for e in extrs if e.impl is not None and e.impl.macro_type == EMacro.DefineEnum]
        for extr in self.cached_extrs:
            sdl_enums = "\n"
            sdl_enum_includes = "\n"
            for enum_def in enum_defs:
                sdl_enums += f"TSdlEnum<typename {enum_def.impl.enum_type}>::getSdlEnum(),\n"
                sdl_enum_includes += f"{enum_def.source_include_expr}\n"
            
            # Just for aesthetics of generated code
            sdl_enums = textwrap.indent(sdl_enums, "                        ")
            sdl_enum_includes = textwrap.indent(sdl_enum_includes, "                ")

            getter_name = extr.user_spec_at(0)

            # Supports namespace scope only--we are not including function declaration so
            # specifying scope (such as class scope) on function name will cause "namespace has no such member" error
            outer_scope_expr = f"::{extr.outer_scope}" if extr.outer_scope else ""

            src.source += textwrap.dedent(
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
            
        self.cached_extrs = []

        return src if src.source else None


class MetaDispatcherForAllSdlClassesHandler(MacroHandler):
    def __init__(self):
        super().__init__()

        self.pybind_extrs = []

    @property
    def macro_name(self):
        return 'PH_DECLARE_DISPATCHER_FOR_ALL_SDL_CLASSES'
    
    def generate_source(self, extr):
        if extr.project_name != "SDLPyBind":
            raise ValueError(f"Cannot use {self.macro_name} in {extr.project_name} project. This macro is for SDLPyBind project only.")

        # For arguments, we need at least `dispatcherFuncName`, `visitorType`, `project`
        if len(extr.user_specs) < 3:
            raise ValueError(f"{self.macro_name}() requires at least 3 arguments, {len(extr.user_specs)} were given")

        self.pybind_extrs.append(extr)
        return None

    def post_generate_source_for_all_projects(self, current_project, project_to_extrs):
        # Only generate for SDLPyBind project
        if current_project != "SDLPyBind":
            return None

        src = MacroImplementation()
        src.macro_type = EMacro.DeclareMeta

        for pybind_extr in self.pybind_extrs:
            project_enum = pybind_extr.user_spec_at(2)
            project_name = project_enum.split("::")[-1]
            if project_name not in project_to_extrs:
                raise ValueError(f"No class definitions for {project_name} project.")
        
            class_defs = [e for e in project_to_extrs[project_name] if e.impl is not None and e.impl.macro_type == EMacro.DefineClass]

            dispatcher_name = pybind_extr.user_spec_at(0)
            visitor_type = pybind_extr.user_spec_at(1)

            visited_sdl_classes = "\n"
            class_includes = "\n"
            for class_def in class_defs:
                visited_sdl_classes += f"visitor(*({class_def.impl.owner_class_type}::getSdlClass()));\n"
                class_includes += f"{class_def.source_include_expr}\n"

            # Just for aesthetics of generated code
            visited_sdl_classes = textwrap.indent(visited_sdl_classes, "                    ")
            class_includes = textwrap.indent(class_includes, "                ")

            func_and_visitor_includes = pybind_extr.source_include_expr
            outer_scope_expr = f"{pybind_extr.outer_scope}::" if pybind_extr.outer_scope else ""

            src.source += textwrap.dedent(
                f"""
                {func_and_visitor_includes}

                {class_includes}

                #include <Engine/Utility/traits.h>

                #include <vector>

                void ph::{outer_scope_expr}{dispatcher_name}({visitor_type}& visitor)
                {{
                    {visited_sdl_classes}
                }}

                """)
            
        self.pybind_extrs = []

        return src if src.source else None


def _extract_macros(project_name: str, source_file: Path, source_dir: Path, handlers: list[MacroHandler]) -> list[MacroExtraction]:
    """
    :returns: A list of `MacroExtraction`.
    """
    # Parse source file line by line to find SDL macro usage
    extractions = []
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

            # Stores something like "flag1, key1=v1;v2, key2=v3"
            str_within_parentheses = line_matches.group(1)

            # Transform into ["flag1", "key1=v1;v2", "key2=v3"]
            tokens = [token.strip() for token in str_within_parentheses.split(',')]

            # Transform into {"flag1": [], "key1": ["v1", "v2"], "key2": ["v3"]}
            user_specs = {}
            for token in tokens:
                if '=' in token:
                    key = token[:token.index('=')]
                    values = [value.strip() for value in token[token.index('=') + 1:].split(';')]
                    user_specs[key] = values
                else:
                    user_specs[token] = []

            extraction = MacroExtraction()
            extraction.handler = handler
            extraction.project_name = project_name
            extraction.source_file = source_file
            extraction.source_dir = source_dir
            extraction.user_specs = user_specs
            extractions.append(extraction)

            # Handlers are unique, skip the rests if we handled one
            break

    return extractions


def _generate_source(extractions: list[MacroExtraction]) -> Union[CompilationUnit, None]:
    """
    Generate source code for a group of macro extractions.

    :returns: A single compilation unit (`CompilationUnit`). `None` if nothing is generated.
    """
    impls = []
    for extraction in extractions:
        extraction.impl = extraction.handler.generate_source(extraction)
        
        if extraction.impl:
            impls.append(extraction.impl)

    if not impls:
        return None

    assert len(impls) > 0
    first_file = extractions[0].source_file
    first_dir = extractions[0].source_dir

    unit = CompilationUnit()
    unit.unique_name = first_file.relative_to(first_dir).with_suffix('').as_posix().replace('/', '_')
    unit.impls = impls
    unit.version_id = uuid.uuid4()
    return unit


def _post_generate_source(extractions: list[MacroExtraction], handlers: list[MacroHandler]) -> list[CompilationUnit]:
    """
    Post generate source code for all macro extractions.

    :param extractions: [in,out] Macro extractions to process. They can also be modified during the process.
    :returns: A list if compilation units generated additionally.
    """
    post_units = []
    for handler in handlers:
        post_impl = handler.post_generate_source(extractions)
        if post_impl:
            post_unit = CompilationUnit()
            post_unit.unique_name = f"post_{handler.macro_name}"
            post_unit.impls = [post_impl]
            post_unit.version_id = uuid.uuid4()
            post_units.append(post_unit)

    return post_units


def _post_generate_source_for_all_projects(current_project: str, project_name_to_extrs: dict[str, list[MacroExtraction]], handlers: list[MacroHandler]) -> list[CompilationUnit]:
    post_units = []
    for handler in handlers:
        post_impl = handler.post_generate_source_for_all_projects(current_project, project_name_to_extrs)
        if post_impl:
            post_unit = CompilationUnit()
            post_unit.unique_name = f"post_cross_project_{handler.macro_name}"
            post_unit.impls = [post_impl]
            post_unit.version_id = uuid.uuid4()
            post_units.append(post_unit)

    return post_units


def _init_macro_handlers() -> list[MacroHandler]:
    handlers = [
        ClassHandler(),
        StructHandler(),
        MethodHandler(),
        StaticMethodHandler(),
        EnumHandler(),
        MetaGetterForAllSdlClassesHandler(),
        MetaGetterForAllSdlEnumsHandler(),
        MetaDispatcherForAllSdlClassesHandler(),
        ]
    
    return handlers


def generate(setup_config: configparser.ConfigParser):
    project_name_to_extractions = {}
    handlers = _init_macro_handlers()

    projects = []
    for name, section in config.get_all_projects(setup_config):
        project_name = name.removeprefix("Project.")
        project_dir = Path(section['ProjectDirectory'])
        source_dir = project_dir / 'Source'
        generated_source_dir = project_dir / 'Generated' / 'SDL'
        log_name = f"[Project {project_name}]"
        projects.append((project_name, source_dir, generated_source_dir, log_name))

    # Execute source code generation process for each project
    for project_name, source_dir, generated_source_dir, log_name in projects:
        # Generate for each header file
        units = []
        extractions = []
        num_source_files = 0
        for item_path in source_dir.rglob('*'):
            if not item_path.is_file():
                continue
           
            num_source_files += 1
            if not item_path.suffix in ('.h', '.cpp', '.ipp', '.tpp'):
                continue

            extraction_group = _extract_macros(project_name, item_path, source_dir, handlers)
            extractions.extend(extraction_group)

            unit = _generate_source(extraction_group)
            if unit is not None:
                units.append(unit)
                
        units.extend(_post_generate_source(extractions, handlers))
        project_name_to_extractions[project_name] = extractions

        print(f"{log_name} Processed {num_source_files} source files")

        # Remove old source
        if generated_source_dir.is_dir():
            for item in generated_source_dir.iterdir():
                if not item.is_file() or item.suffix != '.cpp':
                    raise ValueError(f"unexpected item found in {generated_source_dir}")
                
                item.unlink()

        generated_source_dir.mkdir(parents=True, exist_ok=True)

        if not units:
            continue

        print(f"{log_name} Writing {len(units)} compilation unit(s)...")
            
        # Write new source
        for unit in units:
            (generated_source_dir / f"def_{unit.unique_name}").with_suffix('.cpp').write_text(unit.generate_source_code())
        
    # Cross-project post generation is executed after all projects are processed
    for project_name, source_dir, generated_source_dir, log_name in projects:
        units = _post_generate_source_for_all_projects(project_name, project_name_to_extractions, handlers)
        if not units:
            continue

        print(f"{log_name} Writing {len(units)} cross-project compilation unit(s)...")
            
        # Write new source
        for unit in units:
            (generated_source_dir / f"def_{unit.unique_name}").with_suffix('.cpp').write_text(unit.generate_source_code())
    
    # TODO: indicate CMake reconfigure is required in doc
    # TODO: resolve python.exe cannot be deleted issue when deleting previous venv (error no 13)

    print(f"All definitions generated.")
