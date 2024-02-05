#include "DataIO/IniFile.h"
#include "DataIO/io_exceptions.h"
#include "DataIO/Stream/FormattedTextInputStream.h"
#include "DataIO/Stream/FormattedTextOutputStream.h"

#include <Common/Utility/string_utils.h>
#include <Common/logging.h>

#include <format>

namespace ph
{

PH_DEFINE_INTERNAL_LOG_GROUP(IniFile, DataIO);

IniFile::IniFile() :
	m_sections(),
	m_currentSectionIdx()
{
	// By default, add an empty section and make it current
	m_sections.push_back(IniSection());
	m_currentSectionIdx = 0;
}

IniFile::IniFile(const Path& iniFilePath) :
	IniFile()
{
	append(read(iniFilePath));
}

void IniFile::save(const Path& iniFilePath)
{
	FormattedTextOutputStream output(iniFilePath);

	for(const IniSection& section : m_sections)
	{
		// Special case for global (unnamed) section
		if(section.name.empty())
		{
			// Write nothing
		}
		// Normal section
		else
		{
			output.writeLine("[" + section.name + "]");
		}

		for(const auto& prop : section.keyValPairs)
		{
			output.writeLine(prop.first + "=" + prop.second);
		}
	}
}

void IniFile::setCurrentSection(const std::string_view sectionName, const bool createIfNotExist)
{
	const auto optSectionIdx = findSectionIndex(sectionName);
	if(optSectionIdx)
	{
		setCurrentSection(*optSectionIdx);
	}
	else
	{
		if(createIfNotExist)
		{
			m_sections.push_back(IniSection());
			m_sections.back().name = sectionName;
			setCurrentSection(m_sections.size() - 1);
		}
		else
		{
			throw IOException(std::format("INI section <{}> does not exist", sectionName));
		}
	}
}

std::optional<std::size_t> IniFile::findSectionIndex(const std::string_view sectionName) const
{
	for(std::size_t i = 0; i < m_sections.size(); ++i)
	{
		if(m_sections[i].name == sectionName)
		{
			return i;
		}
	}

	return std::nullopt;
}

std::optional<std::size_t> IniFile::findPropertyIndex(const std::string_view propertyName) const
{
	const IniSection& section = getIniSection(m_currentSectionIdx);
	for(std::size_t i = 0; i < section.keyValPairs.size(); ++i)
	{
		if(section.keyValPairs[i].first == propertyName)
		{
			return i;
		}
	}

	return std::nullopt;
}

void IniFile::setProperty(const std::string_view propertyName, const std::string_view propertyValue, const bool createIfNotExist)
{
	if(propertyName.empty())
	{
		PH_LOG(IniFile, Warning, "use of empty property name is discouraged (associated value: <{}>)",
			propertyValue);
	}

	const auto optPropertyIdx = findPropertyIndex(propertyName);
	if(optPropertyIdx)
	{
		setProperty(*optPropertyIdx, propertyValue);
	}
	else
	{
		IniSection& section = getIniSection(m_currentSectionIdx);
		if(createIfNotExist)
		{
			section.keyValPairs.push_back({std::string(propertyName), std::string(propertyValue)});
		}
		else
		{
			throw IOException(std::format("INI section <{}> already contains property <{}>",
				section.name, propertyName));
		}
	}
}

void IniFile::append(const IniFile& other)
{
	for(const IniSection& otherSection : other.m_sections)
	{
		setCurrentSection(otherSection.name);

		for(const auto& otherProperty : otherSection.keyValPairs)
		{
			setProperty(otherProperty.first, otherProperty.second);
		}
	}
}

IniFile IniFile::read(const Path& iniFilePath)
{
	FormattedTextInputStream inputFile(iniFilePath);

	IniFile     result;
	std::string lineBuffer;
	std::size_t lineNumber = 0;
	while(inputFile)
	{
		inputFile.readLine(&lineBuffer);
		const auto line = string_utils::trim(lineBuffer);
		++lineNumber;

		// Skip blank line and comment
		if(line.empty() || line.front() == ';')
		{
			continue;
		}

		// Parse section if '[' is found
		if(line.front() == '[')
		{
			if(line.back() != ']')
			{
				throw IOException(std::format("on line {}, section without ending bracket", lineNumber));
			}

			std::string_view sectionName = line;

			PH_ASSERT_GT(sectionName.size(), 2);
			sectionName = sectionName.substr(1, sectionName.size() - 2);
			if(sectionName.empty())
			{
				throw IOException(std::format("on line {}, section name is empty", lineNumber));
			}

			result.setCurrentSection(sectionName);

			continue;
		}

		// Parse property if '=' is found
		if(const auto equalCharPos = line.find('='); equalCharPos != std::string::npos)
		{
			const std::string_view propLine = line;
			PH_ASSERT_EQ(propLine[equalCharPos], '=');

			const auto propName  = string_utils::trim(propLine.substr(0, equalCharPos));
			const auto propValue = string_utils::trim(propLine.substr(equalCharPos + 1));

			result.setProperty(propName, propValue);

			continue;
		}
	}

	return result;
}

}// end namespace ph
