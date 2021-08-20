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

void IniFile::setCurrentSection(const std::string_view sectionName)
{
	if(!trySetCurrentSection(sectionName))
	{
		throw IOException(std::format("INI section <{}> does not exist", sectionName));
	}
}

bool IniFile::trySetCurrentSection(const std::string_view sectionName)
{
	const auto optSectionIdx = findSectionIndex(sectionName);
	if(optSectionIdx)
	{
		m_currentSectionIdx = *optSectionIdx;
		return true;
	}
	else
	{
		return false;
	}
}

void IniFile::addSection(std::string sectionName, const bool makeCurrent)
{
	if(findSectionIndex(sectionName))
	{
		throw IOException(std::format("INI section <{}> already exist", sectionName));
	}

	m_sections.push_back(IniSection());
	m_sections.back().name = std::move(sectionName);
	
	if(makeCurrent)
	{
		setCurrentSection(m_sections.size() - 1);
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

std::optional<std::string_view> IniFile::findProperty(const std::string_view propertyName) const
{
	const auto optPropertyIdx = findPropertyIndex(propertyName);
	if(optPropertyIdx)
	{
		return getPropertyValue(*optPropertyIdx);
	}
	else
	{
		return std::nullopt;
	}
}

void IniFile::addProperty(std::string propertyName, std::string propertyValue)
{
	IniSection& section = getIniSection(m_currentSectionIdx);
	if(findPropertyIndex(propertyName))
	{
		throw IOException(std::format("INI section <{}> already contains property <{}>", 
			section.name, propertyName));
	}

	section.keyValPairs.push_back({std::move(propertyName), std::move(propertyValue)});
}

}// end namespace ph
