#pragma once

#include "FileIO/FileSystem/Path.h"
#include "Common/Logger.h"

#include <vector>
#include <string>

namespace ph
{

/*
	Reference: http://lumen.iee.put.poznan.pl/kw/iesna.txt
*/

class IesFile final
{
public:
	enum class EType
	{
		LM_63_1986,
		LM_63_1991,
		LM_63_1995,
		UNKNOWN
	};

public:
	IesFile(const Path& path);

	bool load();

private:
	Path m_path;

	// parsed and exposed data
	//
	EType       m_type;
	std::string m_testInfo;
	std::string m_luminaireManufacturer;
	std::string m_luminaireCatalogNumber;
	std::string m_luminaireDescription;
	std::string m_lampCatalogNumber;
	std::string m_lampDescription;
	std::string m_uncategorizedInfo;

	// parsed but unexposed data
	//
	std::string m_tilt;

	bool parse(const std::vector<char>& data);
	std::size_t parseFileType(const std::vector<std::string>& lines, std::size_t currentLine);
	std::size_t parseLabelsAndKeywords(const std::vector<std::string>& lines, std::size_t currentLine);
	std::size_t parseTiltLine(const std::vector<std::string>& lines, std::size_t currentLine);
	std::size_t parseTiltData(const std::vector<std::string>& lines, std::size_t currentLine);

	EType       getType()                   const;
	std::string getTestInfo()               const;
	std::string getLuminaireManufacturer()  const;
	std::string getLuminaireCatalogNumber() const;
	std::string getLuminaireDescription()   const;
	std::string getLampCatalogNumber()      const;
	std::string getLampDescription()        const;
	std::string getUncategorizedInfo()      const;

private:
	static const Logger logger;

	static std::vector<std::string> retrieveLines(const std::vector<char>& data);
};

}// end namespace ph