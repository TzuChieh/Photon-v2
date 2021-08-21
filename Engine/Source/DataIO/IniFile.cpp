#include "DataIO/IniFile.h"
#include "DataIO/io_exceptions.h"

#include <format>

namespace ph
{

IniFile::IniFile() :
	m_sections(),
	m_currentSectionIdx()
{
	// By default, add an empty section and make it current
	m_sections.push_back(IniSection());
	m_currentSectionIdx = 0;
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

void IniFile::replace(const IniFile& other)
{

}

}// end namespace ph
