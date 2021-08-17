#pragma once

#include "InterfaceGen/InterfaceGenerator.h"

#include <DataIO/Stream/FormattedTextFileOutputStream.h>

#include <string>
#include <utility>
#include <vector>

namespace ph { class SdlField; }
namespace ph { class SdlFunction; }
namespace ph { class SdlEnum; }

namespace ph::sdlgen
{

// TODO: string generation, generate() should base on string output
class MarkdownDocGenerator : public InterfaceGenerator
{
public:
	explicit MarkdownDocGenerator(Path outputDirectory);
	MarkdownDocGenerator(Path outputDirectory, std::string filenameWithoutExt);

	void generate(
		const std::vector<const SdlClass*>& sdlClasses,
		const std::vector<const SdlEnum*>&  sdlEnums) override;

	void writeClassDoc(const SdlClass* sdlClass);
	void writeClassCreationDoc(const SdlClass* sdlClass);
	void writeFunctionDoc(const SdlFunction* sdlFunc, const SdlClass* parentSdlClass = nullptr);
	void writeEnumDoc(const SdlEnum* sdlEnum);
	void writeInputTable(const std::vector<const SdlField*>& inputs);
	void writeLine(std::string line);
	void writeString(std::string str);
	void writeChar(char ch);
	void writeNewLine();
	void clearDoc();
	const std::string& getDoc() const;

	const std::string& getFilename() const;

private:
	FormattedTextFileOutputStream m_file;
	std::string                   m_filename;
	std::string                   m_docString;

	static std::vector<const SdlField*> gatherInputs(const SdlClass* sdlClass);
	static std::vector<const SdlField*> gatherInputs(const SdlFunction* sdlFunc);
};

// In-header Implementations:

inline const std::string& MarkdownDocGenerator::getFilename() const
{
	return m_filename;
}

inline void MarkdownDocGenerator::clearDoc()
{
	m_docString.clear();
}

inline const std::string& MarkdownDocGenerator::getDoc() const
{
	return m_docString;
}

inline void MarkdownDocGenerator::writeLine(std::string line)
{
	writeString(std::move(line));
	writeNewLine();
}

inline void MarkdownDocGenerator::writeNewLine()
{
	writeChar('\n');
}

inline void MarkdownDocGenerator::writeString(std::string str)
{
	m_docString += std::move(str);
}

inline void MarkdownDocGenerator::writeChar(const char ch)
{
	m_docString += ch;
}

}// end namespace ph::sdlgen
