#pragma once

#include "InterfaceGen/InterfaceGenerator.h"

#include <DataIO/Stream/FormattedTextOutputStream.h>

#include <string>
#include <string_view>
#include <utility>
#include <vector>
#include <format>

namespace ph { class SdlField; }
namespace ph { class SdlFunction; }

namespace ph::sdlgen
{

/*! @brief Generate SDL documentation in markdown format.
*/
class MarkdownDocGenerator : public InterfaceGenerator
{
public:
	explicit MarkdownDocGenerator(Path outputDirectory);
	MarkdownDocGenerator(Path outputDirectory, std::string filenameWithoutExt);

	void generate(
		TSpanView<const SdlClass*> sdlClasses,
		TSpanView<const SdlEnum*> sdlEnums) override;

	void writeClassDoc(const SdlClass* sdlClass);
	void writeClassCreationDoc(const SdlClass* sdlClass);
	void writeFunctionDoc(const SdlFunction* sdlFunc, const SdlClass* parentSdlClass = nullptr);
	void writeEnumDoc(const SdlEnum* sdlEnum);
	void writeInputTable(const std::vector<const SdlField*>& inputs);
	void writeLine(std::string_view line);
	void writeString(std::string_view str);
	void writeChar(char ch);
	void writeNewLine();

	template<typename... Args>
	void writeLine(std::string_view formatStr, Args&&... args);

	template<typename... Args>
	void writeString(std::string_view formatStr, Args&&... args);

	void clearDoc();

	const std::string& getDoc() const;
	const std::string& getFilename() const;

private:
	FormattedTextOutputStream m_file;
	std::string               m_filename;
	std::string               m_docString;

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

inline void MarkdownDocGenerator::writeLine(std::string_view line)
{
	writeString(line);
	writeNewLine();
}

inline void MarkdownDocGenerator::writeNewLine()
{
	writeChar('\n');
}

inline void MarkdownDocGenerator::writeString(std::string_view str)
{
	m_docString += str;
}

template<typename... Args>
inline void MarkdownDocGenerator::writeLine(std::string_view formatStr, Args&&... args)
{
	writeLine(
		std::vformat(formatStr, std::make_format_args(std::forward<Args>(args)...)));
}

template<typename... Args>
inline void MarkdownDocGenerator::writeString(std::string_view formatStr, Args&&... args)
{
	writeString(
		std::vformat(formatStr, std::make_format_args(std::forward<Args>(args)...)));
}

inline void MarkdownDocGenerator::writeChar(const char ch)
{
	m_docString += ch;
}

}// end namespace ph::sdlgen
