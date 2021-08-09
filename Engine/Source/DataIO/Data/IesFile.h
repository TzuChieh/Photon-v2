#pragma once

#include "DataIO/FileSystem/Path.h"
#include "Common/primitive_type.h"

#include <vector>
#include <string>

namespace ph
{

/*
	A low-level class that parses an IES photometric data file and provides 
	convenient methods for file content accessing.

	Reference: http://lumen.iee.put.poznan.pl/kw/iesna.txt
*/
class IesFile final
{
public:
	enum class EIesFileType
	{
		LM_63_1986,
		LM_63_1991,
		LM_63_1995,
		UNKNOWN
	};

	enum class EPhotometricWebType
	{
		/*
			Type A photometry is normally used for automotive headlights and signal
			lights. The polar axis of the luminaire coincides with the major axis
			(length) of the luminaire, and the 0-180 degree photometric plane
			coinicides with the luminaire's vertical axis.
		*/
		A, 

		/*
			Type B photometry is normally used for adjustable outdoor area and sports
			lighting luminaires. The polar axis of the luminaire coincides with the
			minor axis (width) of the luminaire, and the 0-180 degree photometric
			plane coinicides with the luminaire's vertical axis.
		*/
		B, 

		/*
			Type C photometry is normally used for architectural and roadway
			luminaires. The polar axis of the photometric web coincides with the
			vertical axis of the luminaire, and the 0-180 degree photometric plane
			coincides with the luminaire's major axis (length).
		*/
		C
	};

public:
	IesFile(const Path& iesFilePath);

	bool load();

	std::string getFilename() const;

	// Retrieving file contents.

	// Gets the type of this IES file.
	//
	EIesFileType getIesFileType()            const;

	// Gets the test report number and laboratory for the IES file.
	//
	std::string  getTestInfo()               const;

	std::string  getLuminaireManufacturer()  const;
	std::string  getLuminaireCatalogNumber() const;
	std::string  getLuminaireDescription()   const;
	std::string  getLampCatalogNumber()      const;
	std::string  getLampDescription()        const;

	// Gets descriptions that are not categorized by the parser.
	//
	std::string  getUncategorizedInfo()      const;

	uint32              getNumLamps()            const;
	real                getLumensPerLamp()       const;
	real                getCandelaMultiplier()   const;
	uint32              getNumVerticalAngles()   const;
	uint32              getNumHorizontalAngles() const;
	EPhotometricWebType getPhotometricWebType()  const;

	std::vector<real>              getVerticalAngles()   const;
	std::vector<real>              getHorizontalAngles() const;

	// Candela values stored as data[horizontal-index][vertical-index].
	//
	std::vector<std::vector<real>> getCandelaValues()    const;

private:
	Path m_path;

	EIesFileType m_iesFileType;

	// file descriptions (labels/keywords in the file)
	//
	std::string m_testInfo;
	std::string m_luminaireManufacturer;
	std::string m_luminaireCatalogNumber;
	std::string m_luminaireDescription;
	std::string m_lampCatalogNumber;
	std::string m_lampDescription;
	std::string m_uncategorizedInfo;

	// type of tilt information
	//
	std::string m_tilt;

	// first line of metadata for the IES file (metadata 1)
	//
	uint32              m_numLamps;
	real                m_lumensPerLamp;
	real                m_candelaMultiplier;
	uint32              m_numVerticalAngles;
	uint32              m_numHorizontalAngles;
	EPhotometricWebType m_webType;
	real                m_luminaireOpeningArea;

	// second line of metadata for the IES file (metadata 2)
	//
	real                m_ballastFactor;
	real                m_ballastLampPhotometricFactor;
	real                m_inputWatts;

	// lighting data
	//
	std::vector<real>              m_verticalAngles;
	std::vector<real>              m_horizontalAngles;
	std::vector<std::vector<real>> m_candelaValues;

	bool parse(const std::vector<char>& data);
	std::size_t parseFileType(const std::vector<std::string>& lines, std::size_t currentLine);
	std::size_t parseLabelsAndKeywords(const std::vector<std::string>& lines, std::size_t currentLine);
	std::size_t parseTiltLine(const std::vector<std::string>& lines, std::size_t currentLine);
	std::size_t parseTiltData(const std::vector<std::string>& lines, std::size_t currentLine);
	std::size_t parseMetadata1(const std::vector<std::string>& lines, std::size_t currentLine);
	std::size_t parseMetadata2(const std::vector<std::string>& lines, std::size_t currentLine);
	std::size_t parseAngles(const std::vector<std::string>& lines, std::size_t currentLine);
	std::size_t parseCandelaValues(const std::vector<std::string>& lines, std::size_t currentLine);

private:
	static std::vector<std::string> retrieveLines(const std::vector<char>& data);
};

}// end namespace ph
