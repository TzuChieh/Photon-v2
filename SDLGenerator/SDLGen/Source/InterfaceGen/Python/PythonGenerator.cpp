#include "InterfaceGen/Python/PythonGenerator.h"
#include "InterfaceGen/Python/PythonClass.h"
#include "InterfaceGen/Python/PythonMethod.h"
#include "ph_sdlgen.h"

#include <ph_cpp_core.h>
#include <Utility/Timestamp.h>
#include <DataIO/io_utils.h>
#include <DataIO/SDL/Introspect/SdlClass.h>
#include <DataIO/SDL/Introspect/SdlFunction.h>
#include <DataIO/SDL/Introspect/SdlField.h>
#include <DataIO/SDL/Introspect/SdlEnum.h>
#include <DataIO/SDL/ETypeCategory.h>
#include <DataIO/SDL/sdl_helpers.h>
#include <Common/logging.h>

#include <utility>

namespace ph::sdlgen
{

PH_DEFINE_INTERNAL_LOG_GROUP(PythonGenerator, SDLGenCLI);

namespace
{

PythonClass gen_sdl_reference_class(const std::string_view categoryName);
PythonClass gen_sdl_creator_class(const SdlClass* sdlClass);
PythonClass gen_sdl_executor_class(const SdlFunction* sdlFunction, const SdlClass* parentClass);
std::string gen_class_name_base(const SdlClass* sdlClass);

}

PythonGenerator::PythonGenerator(Path outputDirectory) :

	InterfaceGenerator("python", std::move(outputDirectory)),

	m_file()
{}

void PythonGenerator::generate(
	const std::vector<const SdlClass*>& sdlClasses,
	const std::vector<const SdlEnum*>&  sdlEnums)
{
	m_file = FormattedTextFileOutputStream(makeOutputFilePath("pysdl.py"));

	m_file.writeString(
		"# ========================================\n"
		"# NOTE: THIS FILE CONTAINS GENERATED CODE \n"
		"#       DO NOT MODIFY                     \n"
		"# ========================================\n");
	m_file.writeString("# last generated: {} \n\n", Timestamp().toString());

	m_file.writeString(
		io_utils::load_text(makeResourcePath("pysdl_base.py")));

	m_file.writeString("\n\n");

	// Generate reference classes (helper for constructing SDL references)
	const std::vector<std::string_view> sdlCategories = sdl::acquire_categories();
	for(const auto sdlCategory : sdlCategories)
	{
		PythonClass pyClass = gen_sdl_reference_class(sdlCategory);
		m_file.writeString(pyClass.genCode());
	}

	PH_LOG(PythonGenerator, "generated {} helper reference classes", 
		sdlCategories.size());

	m_file.writeString("\n\n");

	std::size_t numSdlCreatorClasses = 0;
	std::size_t numSdlExecutionClasses = 0;
	for(auto const sdlClass : sdlClasses)
	{
		// Generate creator class (only non-blueprint class can be created)
		if(!sdlClass->isBlueprint())
		{
			const PythonClass pyClass = gen_sdl_creator_class(sdlClass);
			m_file.writeString(pyClass.genCode());

			++numSdlCreatorClasses;
		}

		// Generate function execution classes
		for(std::size_t funcIdx = 0; funcIdx < sdlClass->numFunctions(); ++funcIdx)
		{
			auto const sdlFunc = sdlClass->getFunction(funcIdx);
			const PythonClass pyClass = gen_sdl_executor_class(sdlFunc, sdlClass);
			m_file.writeString(pyClass.genCode());

			++numSdlExecutionClasses;
		}
	}

	PH_LOG(PythonGenerator, "generated {} creator classes and {} execution classes", 
		numSdlCreatorClasses, numSdlExecutionClasses);
}

Path PythonGenerator::makeResourcePath(const std::string& fileSubPath) const
{
	return get_internal_resource_directory(EEngineProject::SDLGen)
		.append(Path("PythonGenerator"))
		.append(Path(fileSubPath));
}

namespace
{

PythonClass gen_sdl_reference_class(const std::string_view categoryName)
{
	PythonClass clazz("SDL" + sdl_name_to_camel_case(categoryName, true));
	clazz.setInheritedClass("SDLReference");

	PythonMethod initMethod("__init__");
	initMethod.addInput("ref_name", "\"\"");
	initMethod.addCodeLine("super().__init__(\"{}\", ref_name)", categoryName);
	clazz.addMethod(initMethod);

	return clazz;
}

PythonClass gen_sdl_creator_class(const SdlClass* const sdlClass)
{
	PH_ASSERT(sdlClass);
	PH_ASSERT(!sdlClass->isBlueprint());

	PythonClass clazz(gen_class_name_base(sdlClass) + "Creator");
	clazz.setInheritedClass("SDLCreatorCommand");
	clazz.addDefaultInit();

	// Override get_full_type()
	PythonMethod fullTypeMethod("get_full_type");
	fullTypeMethod.addCodeLine("return \"{}({})\"", 
		sdlClass->genCategoryName(), sdlClass->getTypeName());
	clazz.addMethod(fullTypeMethod);

	// Add input methods
	for(std::size_t i = 0; i < sdlClass->numFields(); ++i)
	{
		const SdlField* const field = sdlClass->getField(i);
		PH_ASSERT(field);

		const auto fieldName = sdl_name_to_snake_case(field->getFieldName());

		PythonMethod inputMethod("set_" + fieldName);
		inputMethod.addInput(fieldName, "", "SDLData");
		inputMethod.addCodeLine("self.set_input(\"{}\", {})", field->getFieldName(), fieldName);
		clazz.addMethod(inputMethod);
	}

	return clazz;
}

PythonClass gen_sdl_executor_class(const SdlFunction* const sdlFunction, const SdlClass* const parentClass)
{
	PH_ASSERT(sdlFunction);
	PH_ASSERT(parentClass);

	PythonClass clazz(
		gen_class_name_base(parentClass) +
		sdl_name_to_camel_case(sdlFunction->getName(), true));
	clazz.setInheritedClass("SDLExecutorCommand");
	clazz.addDefaultInit();

	// Override get_full_type()
	PythonMethod fullTypeMethod("get_full_type");
	fullTypeMethod.addCodeLine("return \"{}({})\"",
		parentClass->genCategoryName(), parentClass->getTypeName());
	clazz.addMethod(fullTypeMethod);

	// Override get_name()
	PythonMethod getNameMethod("get_name");
	getNameMethod.addCodeLine("return \"{}\"", sdlFunction->getName());
	clazz.addMethod(getNameMethod);

	// Add input methods
	for(std::size_t i = 0; i < sdlFunction->numParams(); ++i)
	{
		const SdlField* const param = sdlFunction->getParam(i);
		PH_ASSERT(param);

		const auto paramName = sdl_name_to_snake_case(param->getFieldName());

		PythonMethod inputMethod("set_" + paramName);
		inputMethod.addInput(paramName, "", "SDLData");
		inputMethod.addCodeLine("self.set_input(\"{}\", {})", param->getFieldName(), paramName);
		clazz.addMethod(inputMethod);
	}

	return clazz;
}

std::string gen_class_name_base(const SdlClass* const sdlClass)
{
	PH_ASSERT(sdlClass);

	const std::string typePart     = sdl_name_to_camel_case(sdlClass->getTypeName(), true);
	const std::string categoryPart = sdl_name_to_camel_case(sdlClass->genCategoryName(), true);

	return typePart + categoryPart;
}

}

}// end namespace ph::sdlgen
