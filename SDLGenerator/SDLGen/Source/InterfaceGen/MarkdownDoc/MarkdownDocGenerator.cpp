#include "InterfaceGen/MarkdownDoc/MarkdownDocGenerator.h"
#include "ph_sdlgen.h"

#include <DataIO/SDL/Introspect/SdlClass.h>
#include <DataIO/SDL/Introspect/SdlFunction.h>
#include <DataIO/SDL/Introspect/SdlField.h>
#include <DataIO/SDL/Introspect/SdlEnum.h>
#include <DataIO/SDL/sdl_helpers.h>
#include <Common/assertion.h>

#include <format>

namespace ph::sdlgen
{

MarkdownDocGenerator::MarkdownDocGenerator(Path outputDirectory) :
	MarkdownDocGenerator(std::move(outputDirectory), "untitled-markdown")
{}

MarkdownDocGenerator::MarkdownDocGenerator(Path outputDirectory, std::string filenameWithoutExt) :

	InterfaceGenerator("markdown-doc", std::move(outputDirectory)),

	m_file     (),
	m_filename (std::move(filenameWithoutExt)),
	m_docString()
{
	if(m_filename.empty())
	{
		m_filename = "untitled-markdown";
	}
	
	m_filename += ".md";
}

void MarkdownDocGenerator::generate(
	const std::vector<const SdlClass*>& sdlClasses,
	const std::vector<const SdlEnum*>&  sdlEnums)
{
	clearDoc();

	writeLine("# Photon Scene Description Language");

	writeNewLine();

	for(const SdlClass* const sdlClass : sdlClasses)
	{
		writeClassDoc(sdlClass);

		writeNewLine();
	}

	writeNewLine();

	for(const SdlEnum* const sdlEnum : sdlEnums)
	{
		writeEnumDoc(sdlEnum);

		writeNewLine();
	}

	writeNewLine();

	// Finally, write the doc to file
	m_file = FormattedTextFileOutputStream(makeOutputFilePath(getFilename()));
	m_file.writeString(getDoc());
}

void MarkdownDocGenerator::writeClassDoc(const SdlClass* const sdlClass)
{
	if(!sdlClass)
	{
		return;
	}

	writeLine("## " + sdlClass->getDocName());

	writeNewLine();
	
	writeLine("* Category: `" + sdl_name_to_capitalized(sdl::category_to_string(sdlClass->getCategory())) + "`");
	writeLine("* Type: `" + sdl_name_to_capitalized(sdlClass->getTypeName()) + "`");

	// Write notes for the SDL class
	{
		writeString("* Note: ");

		if(sdlClass->isBlueprint())
		{
			writeString("**blueprint only**");
		}
		else
		{
			writeString("**concrete**");
		}

		if(sdlClass->isDerived())
		{
			// TODO: link to base SDL class

			const SdlClass* const baseSdlClass = sdlClass->getBase();
			PH_ASSERT(baseSdlClass);

			writeString(", based on **" + baseSdlClass->getDocName() + "**");
		}

		writeNewLine();
	}
	
	writeNewLine();

	if(!(sdlClass->getDescription().empty()))
	{
		writeLine(sdlClass->getDescription());
	}
	else
	{
		writeLine("*(no description)*");
	}

	writeNewLine();

	writeClassCreationDoc(sdlClass);

	writeNewLine();

	// Write documentation for every function in the class
	for(std::size_t funcIdx = 0; funcIdx < sdlClass->numFunctions(); ++funcIdx)
	{
		writeFunctionDoc(sdlClass->getFunction(funcIdx), sdlClass);

		writeNewLine();
	}
}

void MarkdownDocGenerator::writeClassCreationDoc(const SdlClass* const sdlClass)
{
	if(!sdlClass)
	{
		return;
	}

	// Only non-blueprint classes can be created
	if(!(sdlClass->isBlueprint()))
	{
		const auto creationalTypeName = std::format("`{}({})`",
			sdl::category_to_string(sdlClass->getCategory()), sdlClass->getTypeName());

		writeLine("> Creation: " + creationalTypeName);

		writeNewLine();
	}

	writeInputTable(gatherInputs(sdlClass));
}

void MarkdownDocGenerator::writeFunctionDoc(const SdlFunction* const sdlFunc, const SdlClass* const parentSdlClass)
{
	if(!sdlFunc)
	{
		return;
	}

	std::string callableTypeName;
	if(parentSdlClass)
	{
		callableTypeName = std::format("callable on `{}({})` and its derivations",
			sdl::category_to_string(parentSdlClass->getCategory()), parentSdlClass->getTypeName());
	}
	else
	{
		callableTypeName = "*(callable type not required)*";
	}
	

	writeLine("> Operation: `" + sdlFunc->getName() + "`, " + callableTypeName);

	writeNewLine();

	writeLine(sdlFunc->getDescription());

	writeNewLine();

	writeInputTable(gatherInputs(sdlFunc));
}

void MarkdownDocGenerator::writeEnumDoc(const SdlEnum* const sdlEnum)
{
	if(!sdlEnum)
	{
		return;
	}

	writeLine("## " + sdl_name_to_capitalized(sdlEnum->getName()));

	writeNewLine();

	writeLine(sdlEnum->getDescription());

	writeNewLine();

	if(sdlEnum->numEntries() == 0)
	{
		writeLine("(no entry)");
	}
	else
	{
		writeLine("| Entries | Descriptions |");
		writeLine("| --- | --- |");

		for(std::size_t entryIdx = 0; entryIdx < sdlEnum->numEntries(); ++entryIdx)
		{
			const SdlEnum::Entry entry = sdlEnum->getEntry(entryIdx);

			// TODO: link for type doc

			std::string entryName(entry.name);
			if(!entryName.empty())
			{
				entryName = "`" + entryName + "`";
			}
			else
			{
				entryName = "*(empty)*";
			}

			std::string entryDescription(sdlEnum->getEntryDescription(entryIdx));
			if(entryDescription.empty())
			{
				entryDescription = "*(no description)*";
			}

			writeLine("| " + entryName + " | " + entryDescription + " |");
		}
	}
}

void MarkdownDocGenerator::writeInputTable(const std::vector<const SdlField*>& inputs)
{
	if(inputs.empty())
	{
		writeLine("*(no input)*");
	}
	else
	{
		writeLine("| Inputs | Types | Descriptions |");
		writeLine("| --- | --- | --- |");

		for(const SdlField* const field : inputs)
		{
			if(!field)
			{
				continue;
			}

			// TODO: link for type doc

			const auto& fieldName   = field->getFieldName();
			const auto& typeName    = field->getTypeName();
			const auto& description = field->getDescription();

			writeLine("| " + fieldName + " | `" + typeName + "` | " + description + " |");
		}
	}
}

std::vector<const SdlField*> MarkdownDocGenerator::gatherInputs(const SdlClass* const sdlClass)
{
	if(!sdlClass)
	{
		return {};
	}

	std::vector<const SdlField*> inputs(sdlClass->numFields());
	for(std::size_t i = 0; i < sdlClass->numFields(); ++i)
	{
		inputs[i] = sdlClass->getField(i);
	}

	return inputs;
}

std::vector<const SdlField*> MarkdownDocGenerator::gatherInputs(const SdlFunction* const sdlFunc)
{
	if(!sdlFunc)
	{
		return {};
	}

	std::vector<const SdlField*> inputs(sdlFunc->numParams());
	for(std::size_t i = 0; i < sdlFunc->numParams(); ++i)
	{
		inputs[i] = sdlFunc->getParam(i);
	}

	return inputs;
}

}// end namespace ph::sdlgen
