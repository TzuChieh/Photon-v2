#pragma once

#include "DataIO/FileSystem/Path.h"
#include "Common/assertion.h"

#include <string>
#include <string_view>
#include <vector>
#include <cstddef>
#include <optional>
#include <utility>

namespace ph
{

class IniFile final
{
public:
	static IniFile read(const Path& iniFilePath);

public:
	IniFile();
	explicit IniFile(const Path& iniFilePath);

	void save(const Path& iniFilePath);
	void clear();

	std::size_t numSections() const;
	std::string_view getSectionName(std::size_t sectionIdx) const;
	std::string_view getCurrentSectionName() const;
	std::optional<std::size_t> findSectionIndex(std::string_view sectionName) const;
	void setCurrentSection(std::size_t sectionIdx);
	void setCurrentSection(std::string_view sectionName, bool createIfNotExist = true);
	
	std::size_t numProperties() const;
	std::string_view getPropertyName(std::size_t propertyIdx) const;
	std::string_view getPropertyValue(std::size_t propertyIdx) const;
	std::optional<std::size_t> findPropertyIndex(std::string_view propertyName) const;
	void setProperty(std::size_t propertyIdx, std::string_view propertyValue);
	void setProperty(std::string_view propertyName, std::string_view propertyValue, bool createIfNotExist = true);

	/*! @brief Add another INI file to this one.
	All properties from the other file will be added to this one. New sections
	will be created if they were not in this file. Properties will be overwritten
	if they were already defined in this file.
	*/
	void append(const IniFile& other);

private:
	struct IniSection final
	{
		std::string name;
		std::vector<std::pair<std::string, std::string>> keyValPairs;
	};

	std::vector<IniSection> m_sections;
	std::size_t             m_currentSectionIdx;

	IniSection& getIniSection(std::size_t sectionIdx);
	const IniSection& getIniSection(std::size_t sectionIdx) const;
};

// In-header Implementations:

inline std::size_t IniFile::numSections() const
{
	return m_sections.size();
}
	
inline std::size_t IniFile::numProperties() const
{
	return getIniSection(m_currentSectionIdx).keyValPairs.size();
}

inline std::string_view IniFile::getSectionName(const std::size_t sectionIdx) const
{
	return getIniSection(sectionIdx).name;
}

inline std::string_view IniFile::getCurrentSectionName() const
{
	return getIniSection(m_currentSectionIdx).name;
}

inline void IniFile::setCurrentSection(const std::size_t sectionIdx)
{
	PH_ASSERT_LT(sectionIdx, m_sections.size());

	m_currentSectionIdx = sectionIdx;
}

inline std::string_view IniFile::getPropertyName(const std::size_t propertyIdx) const
{
	const IniSection& section = getIniSection(m_currentSectionIdx);

	PH_ASSERT_LT(propertyIdx, section.keyValPairs.size());
	return section.keyValPairs[propertyIdx].first;
}

inline std::string_view IniFile::getPropertyValue(const std::size_t propertyIdx) const
{
	const IniSection& section = getIniSection(m_currentSectionIdx);

	PH_ASSERT_LT(propertyIdx, section.keyValPairs.size());
	return section.keyValPairs[propertyIdx].second;
}

inline void IniFile::setProperty(const std::size_t propertyIdx, const std::string_view propertyValue)
{
	IniSection& section = getIniSection(m_currentSectionIdx);

	PH_ASSERT_LT(propertyIdx, section.keyValPairs.size());
	section.keyValPairs[propertyIdx].second = propertyValue;
}

inline IniFile::IniSection& IniFile::getIniSection(const std::size_t sectionIdx)
{
	PH_ASSERT_LT(sectionIdx, m_sections.size());

	return m_sections[sectionIdx];
}

inline const IniFile::IniSection& IniFile::getIniSection(const std::size_t sectionIdx) const
{
	PH_ASSERT_LT(sectionIdx, m_sections.size());

	return m_sections[sectionIdx];
}

inline void IniFile::clear()
{
	m_sections.clear();
}

}// end namespace ph
