#include "InterfaceGen/Python/PythonGenerator.h"
#include "InterfaceGen/Python/PythonClass.h"
#include "InterfaceGen/Python/PythonMethod.h"
#include "ph_sdlgen.h"

#include <ph_core.h>
#include <Common/Utility/Timestamp.h>
#include <DataIO/io_utils.h>
#include <SDL/Introspect/SdlClass.h>
#include <SDL/Introspect/SdlFunction.h>
#include <SDL/Introspect/SdlField.h>
#include <SDL/Introspect/SdlEnum.h>
#include <SDL/ESdlTypeCategory.h>
#include <SDL/sdl_helpers.h>
#include <Common/logging.h>
#include <Common/config.h>

#include <utility>
#include <vector>
#include <unordered_set>
#include <unordered_map>

namespace ph::sdlgen
{

PH_DEFINE_INTERNAL_LOG_GROUP(PythonGenerator, SDLGenCLI);

namespace
{

PythonClass gen_sdl_reference_class(const std::string_view categoryName);
PythonClass gen_sdl_creator_class(const SdlClass& sdlClass);
PythonClass gen_sdl_explicit_executor_class(const SdlFunction& sdlFunction, const SdlClass& parentClass);

/*!
A helper explicit executor class simply inherits from the class generated by
`gen_sdl_explicit_executor_class()`. This is useful when classes other than
the original `parentClass` are allowed to call the same `sdlFunction`.
@param callingParentClass Additional class that is allowed to call the function.
*/
PythonClass gen_sdl_helper_explicit_executor_class(
	const SdlFunction& sdlFunction, 
	const SdlClass& parentClass,
	const SdlClass& callingParentClass);

/*!
Implicit SDL executor is calling an executor without specifying its class type. The class type 
is deduced from the target reference.
*/
std::vector<PythonClass> gen_sdl_implicit_executor_classes(TSpanView<const SdlFunction*> sdlFunctions);

PythonClass gen_sdl_version_directive_class();

std::string gen_class_name_base(const SdlClass& sdlClass);
std::string gen_creator_class_name(const SdlClass& sdlClass);
std::string gen_explicit_executor_class_name(const SdlFunction& sdlFunction, const SdlClass& parentClass);

/*!
For a list of input (unresolved) classes, try to resolve inheritances by reordering the classes from
base to derived so code generation can simply be a linear sweep later on.
*/
std::vector<PythonClass> resolve_class_inheritances(const std::vector<PythonClass>& unresolvedClasses);

}// end anonymous namespace

PythonGenerator::PythonGenerator(Path outputDirectory) :

	InterfaceGenerator("python", std::move(outputDirectory)),

	m_file()
{}

void PythonGenerator::generate(
	TSpanView<const SdlClass*> sdlClasses,
	TSpanView<const SdlEnum*> sdlEnums)
{
	m_file = FormattedTextOutputStream(makeOutputFilePath("pysdl.py"));

	m_file.writeString(
		"# ========================================\n"
		"# NOTE: THIS FILE CONTAINS GENERATED CODE \n"
		"#       DO NOT MODIFY                     \n"
		"# ========================================\n");
	m_file.writeString("# engine version: {}\n", PH_ENGINE_VERSION);
	m_file.writeString("# SDL version: {}\n", PH_PSDL_VERSION);
	m_file.writeString("# last generated: {}\n\n", Timestamp().toYMDHMS());

	m_file.writeString(
		io_utils::load_text(makeResourcePath("pysdl_base.py")));

	m_file.writeString("\n\n");

	// Generate reference classes (helper for constructing SDL references)
	{
		const std::vector<std::string_view> sdlCategories = sdl::acquire_categories();
		for(const auto sdlCategory : sdlCategories)
		{
			PythonClass pyClass = gen_sdl_reference_class(sdlCategory);
			m_file.writeString(pyClass.genCode());
		}

		m_file.writeString("\n\n");

		PH_LOG(PythonGenerator, Note, "generated {} helper reference classes", sdlCategories.size());
	}

	// Generate directive classes
	{
		std::vector<PythonClass> directiveClasses;
		directiveClasses.push_back(gen_sdl_version_directive_class());

		for(const auto& pyClass : directiveClasses)
		{
			m_file.writeString(pyClass.genCode());
		}

		m_file.writeString("\n\n");

		PH_LOG(PythonGenerator, Note, "generated {} directive classes", directiveClasses.size());
	}

	// Generate creator classes (for SDL creation command)
	{
		std::vector<PythonClass> creatorClasses;
		for(auto const sdlClass : sdlClasses)
		{
			PH_ASSERT(sdlClass);
			creatorClasses.push_back(gen_sdl_creator_class(*sdlClass));
		}

		creatorClasses = resolve_class_inheritances(creatorClasses);
		for(const PythonClass& creatorClass : creatorClasses)
		{
			m_file.writeString(creatorClass.genCode());
		}

		m_file.writeString("\n\n");

		PH_LOG(PythonGenerator, Note, "generated {} creator classes", creatorClasses.size());
	}

	// Generate function execution classes (for SDL execution command in explicit class type form)
	std::vector<PythonClass> executorClasses;
	for(auto const sdlClass : sdlClasses)
	{
		PH_ASSERT(sdlClass);
		for(std::size_t fi = 0; fi < sdlClass->numFunctions(); ++fi)
		{
			auto const sdlFunc = sdlClass->getFunction(fi);
			PH_ASSERT(sdlFunc);

			executorClasses.push_back(gen_sdl_explicit_executor_class(*sdlFunc, *sdlClass));
		}

		// Generate helper classes for additional functions that is callable from `sdlClass`
		for(auto [sdlFunc, parentSdlClass] : sdl::get_all_callable_functions(sdlClass->getBase()))
		{
			executorClasses.push_back(gen_sdl_helper_explicit_executor_class(*sdlFunc, *parentSdlClass, *sdlClass));
		}
	}

	// Generate function execution classes (for SDL execution command in implicit class type form)
	{
		std::vector<const SdlFunction*> allSdlFunctions;
		for(auto const sdlClass : sdlClasses)
		{
			for(std::size_t fi = 0; fi < sdlClass->numFunctions(); ++fi)
			{
				allSdlFunctions.push_back(sdlClass->getFunction(fi));
			}
		}

		std::vector<PythonClass> implicitExecutorClasses = gen_sdl_implicit_executor_classes(allSdlFunctions);
		executorClasses.insert(executorClasses.end(), implicitExecutorClasses.begin(), implicitExecutorClasses.end());
	}

	executorClasses = resolve_class_inheritances(executorClasses);
	for(const PythonClass& executorClass : executorClasses)
	{
		m_file.writeString(executorClass.genCode());
	}

	m_file.writeString("\n\n");

	PH_LOG(PythonGenerator, Note, "generated {} executor classes", executorClasses.size());
}

Path PythonGenerator::makeResourcePath(const std::string& fileSubPath) const
{
	return get_internal_resource_directory(EEngineProject::SDLGen)
		.append(Path("PythonGenerator"))
		.append(Path(fileSubPath));
}

namespace
{

inline PythonClass gen_sdl_reference_class(const std::string_view categoryName)
{
	PythonClass clazz(sdl::name_to_camel_case(categoryName, true));
	clazz.setInheritedClass("Reference");

	PythonMethod initMethod("__init__");
	initMethod.addInput("ref_name", "\"\"");
	initMethod.addCodeLine("super().__init__(\"{}\", ref_name)", categoryName);
	clazz.addMethod(initMethod);

	return clazz;
}

inline PythonClass gen_sdl_creator_class(const SdlClass& sdlClass)
{
	PythonClass clazz(gen_creator_class_name(sdlClass));

	if(!sdlClass.getBase())
	{
		clazz.setInheritedClass("CreatorCommand");
	}
	// For SDL class with base, inherit from its Python creator class
	else
	{
		clazz.setInheritedClass(gen_creator_class_name(*sdlClass.getBase()));
	}

	clazz.setDoc(std::string(sdlClass.getDescription()));
	clazz.addDefaultInit();

	// Override `get_full_type()` (for non-blueprint class only, so blueprint classes remain
	// uninstantiable since they now has unimplemented abstract method)
	if(!sdlClass.isBlueprint())
	{
		PythonMethod fullTypeMethod("get_full_type");
		fullTypeMethod.addCodeLine("return \"{}({})\"", 
			sdlClass.genCategoryName(), sdlClass.getTypeName());
		clazz.addMethod(fullTypeMethod);
	}

	// Add input methods
	for(std::size_t i = 0; i < sdlClass.numFields(); ++i)
	{
		const SdlField& field = *sdlClass.getField(i);
		const auto fieldName = sdl::name_to_snake_case(field.getFieldName());

		PythonMethod inputMethod("set_" + fieldName);
		inputMethod.setDoc(std::string(field.getDescription()));
		inputMethod.addInput(fieldName, "", "AbstractData");
		inputMethod.addCodeLine("self.set_input(\"{}\", {})", field.getFieldName(), fieldName);
		clazz.addMethod(inputMethod);
	}

	return clazz;
}

inline PythonClass gen_sdl_explicit_executor_class(const SdlFunction& sdlFunction, const SdlClass& parentClass)
{
	PythonClass clazz(gen_explicit_executor_class_name(sdlFunction, parentClass));
	clazz.setInheritedClass("ExplicitExecutorCommand");
	clazz.setDoc(std::string(sdlFunction.getDescription()));
	clazz.addDefaultInit();

	// Override `get_full_type()`
	PythonMethod fullTypeMethod("get_full_type");
	fullTypeMethod.addCodeLine("return \"{}({})\"",
		parentClass.genCategoryName(), parentClass.getTypeName());
	clazz.addMethod(fullTypeMethod);

	// Override `get_name()`
	PythonMethod getNameMethod("get_name");
	getNameMethod.addCodeLine("return \"{}\"", sdlFunction.getName());
	clazz.addMethod(getNameMethod);

	// Add input methods
	for(std::size_t i = 0; i < sdlFunction.numParams(); ++i)
	{
		const SdlField& param = *sdlFunction.getParam(i);
		const auto paramName = sdl::name_to_snake_case(param.getFieldName());

		PythonMethod inputMethod("set_" + paramName);
		inputMethod.setDoc(std::string(param.getDescription()));
		inputMethod.addInput(paramName, "", "AbstractData");
		inputMethod.addCodeLine("self.set_input(\"{}\", {})", param.getFieldName(), paramName);
		clazz.addMethod(inputMethod);
	}

	return clazz;
}

inline PythonClass gen_sdl_helper_explicit_executor_class(
	const SdlFunction& sdlFunction,
	const SdlClass& parentClass,
	const SdlClass& callingParentClass)
{
	// We do not want to generate a helper executor class with the same parent class, calling class 
	// should be a different one
	PH_ASSERT(&parentClass != &callingParentClass);

	PythonClass clazz(gen_explicit_executor_class_name(sdlFunction, callingParentClass));
	clazz.setInheritedClass(gen_explicit_executor_class_name(sdlFunction, parentClass));
	clazz.setDoc(std::string(sdlFunction.getDescription()));
	clazz.addDefaultInit();

	// We do not have to override `get_full_type()` and `get_name()`. Calling with compatible 
	// calling class is equivalent to call on the original parent class. 
	// We do not have to generate input methods also--they are already there due to inheritance.

	return clazz;
}

inline std::vector<PythonClass> gen_sdl_implicit_executor_classes(TSpanView<const SdlFunction*> sdlFunctions)
{
	// The goal here is to generate a Python class for each "function name". Input SDL functions may well 
	// have duplicated names (executors with the same name may be defined by different SDL classes).
	// To keep the spirit of implicit executor syntax, we merge their input parameters together when 
	// multiple SDL functions have the same name. This is fine--see the "Add input methods" section below.
	//
	std::unordered_map<std::string, PythonClass> funcNameToClass;

	for(const SdlFunction* sdlFunction : sdlFunctions)
	{
		PH_ASSERT(sdlFunction);
		if(!sdlFunction)
		{
			continue;
		}

		const auto funcName = std::string(sdlFunction->getName());

		// Only add a new Python class if it does not exist yet
		if(!funcNameToClass.contains(funcName))
		{
			PythonClass clazz("Call" + sdl::name_to_camel_case(funcName, true));
			clazz.setInheritedClass("ImplicitExecutorCommand");
			clazz.setDoc(std::string(sdlFunction->getDescription()));
			clazz.addDefaultInit();

			// Override `get_name()`
			PythonMethod getNameMethod("get_name");
			getNameMethod.addCodeLine("return \"{}\"", funcName);
			clazz.addMethod(getNameMethod);

			funcNameToClass[funcName] = std::move(clazz);
		}

		PythonClass& clazz = funcNameToClass[funcName];

		// Add input methods
		for(std::size_t i = 0; i < sdlFunction->numParams(); ++i)
		{
			const SdlField& param = *sdlFunction->getParam(i);
			const auto paramName = sdl::name_to_snake_case(param.getFieldName());

			// Not adding the input method if it already exists. The Python input method is implemented
			// as taking `AbstractData` as type hint, which is a type that all input parameter types are
			// inherited from. If an input parameter's name is colliding with an already merged one but
			// with a different parameter type, the generated method will be exactly the same since we
			// are already taking `AbstractData`. 
			//
			std::string methodName = "set_" + paramName;
			if(clazz.hasMethod(methodName))
			{
				continue;
			}

			PythonMethod inputMethod("set_" + paramName);
			inputMethod.setDoc(std::string(param.getDescription()));
			inputMethod.addInput(paramName, "", "AbstractData");
			inputMethod.addCodeLine("self.set_input(\"{}\", {})", param.getFieldName(), paramName);
			clazz.addMethod(inputMethod);
		}
	}

	// Note that generated classes is likely to have more input methods than their "unmerged" version.
	// It is programmer's responsibility to use them properly, as this is the implicit form--if not sure,
	// just use the explicit form.

	std::vector<PythonClass> classes;
	classes.reserve(funcNameToClass.size());
	for(auto& [funcName, clazz] : funcNameToClass)
	{
		classes.push_back(std::move(clazz));
	}

	return classes;
}

inline PythonClass gen_sdl_version_directive_class()
{
	PythonClass clazz("VersionDirectiveCommand");
	clazz.setInheritedClass("DirectiveCommand");

	// Set version on init, or default to the current PSDL version
	PythonMethod initMethod("__init__");
	initMethod.addInput("version", "None", "str");
	initMethod.addCodeLine("super().__init__()");
	initMethod.addCodeLine("self.__version = \"{}\"", PH_PSDL_VERSION);
	initMethod.addCodeLine("if version is not None:");
	initMethod.beginIndent();
	{
		initMethod.addCodeLine("self.__version = version");
	}
	initMethod.endIndent();
	initMethod.addCodeLine("self.append_directive(\"version %s\" % self.__version)");
	clazz.addMethod(initMethod);

	return clazz;
}

inline std::string gen_class_name_base(const SdlClass& sdlClass)
{
	const std::string typePart     = sdl::name_to_camel_case(sdlClass.getTypeName(), true);
	const std::string categoryPart = sdl::name_to_camel_case(sdlClass.genCategoryName(), true);

	return typePart + categoryPart;
}

inline std::string gen_creator_class_name(const SdlClass& sdlClass)
{
	return gen_class_name_base(sdlClass) + "Creator";
}

inline std::string gen_explicit_executor_class_name(const SdlFunction& sdlFunction, const SdlClass& parentClass)
{
	return gen_class_name_base(parentClass) +
	       sdl::name_to_camel_case(sdlFunction.getName(), true);
}

inline std::vector<PythonClass> resolve_class_inheritances(const std::vector<PythonClass>& inUnresolvedClasses)
{
	PH_LOG(PythonGenerator, Note, 
		"resolving class inheritances for {} classes", inUnresolvedClasses.size());

	std::vector<PythonClass> resolvedClasses;

	// Start by dividing interfaces into resolved and unresolved
	std::unordered_set<std::string> resolvedClassNames;
	std::vector<PythonClass> unresolvedClasses;
	for(const PythonClass& clazz : inUnresolvedClasses)
	{
		const bool hasResolved =
			!clazz.isInheriting() || 
			clazz.getInheritedClassName() == "CreatorCommand" ||
			clazz.getInheritedClassName() == "ExplicitExecutorCommand" ||
			clazz.getInheritedClassName() == "ImplicitExecutorCommand";

		if(!hasResolved)
		{
			unresolvedClasses.push_back(clazz);
		}
		else
		{
			resolvedClasses.push_back(clazz);
			resolvedClassNames.insert(clazz.getClassName());
		}
	}

	// Iteratively resolve unresolved classes by placing them after resolved ones
	while(!unresolvedClasses.empty())
	{
		bool hasProgress = false;
		for(auto ucIter = unresolvedClasses.begin(); ucIter != unresolvedClasses.end();)
		{
			const auto baseClassName = ucIter->getInheritedClassName();
			const bool hasBaseResolved = resolvedClassNames.find(baseClassName) != resolvedClassNames.end();
			if(!hasBaseResolved)
			{
				++ucIter;
				continue;
			}
			else
			{
				resolvedClasses.push_back(*ucIter);
				resolvedClassNames.insert(ucIter->getClassName());
				
				// Be aware that we are erasing while iterating
				ucIter = unresolvedClasses.erase(ucIter);

				hasProgress = true;
			}
		}

		if(!hasProgress)
		{
			break;
		}
	}

	if(!unresolvedClasses.empty())
	{
		std::string nameListing;
		for(const PythonClass& clazz : unresolvedClasses)
		{
			nameListing += "<" + clazz.getClassName() + "> ";
		}

		PH_LOG(PythonGenerator, Warning,
			"found {} unresolved classes, listing: {}", unresolvedClasses.size(), nameListing);
	}

	return resolvedClasses;
}

}// end anonymous namespace

}// end namespace ph::sdlgen
