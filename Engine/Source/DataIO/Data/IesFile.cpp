#include "DataIO/Data/IesFile.h"
#include "Common/assertion.h"
#include "Common/logging.h"
#include "SDL/Tokenizer.h"

#include <fstream>
#include <vector>
#include <string>

namespace ph
{

PH_DEFINE_INTERNAL_LOG_GROUP(IesFile, DataIO);

IesFile::IesFile(const Path& iesFilePath) :
	m_path(iesFilePath),
	m_iesFileType(EIesFileType::Unknown)
{}

bool IesFile::load()
{
	const std::string filePathStr = m_path.toAbsoluteString();

	// In an IES file, all lines shall be terminated with a <CR><LF> pair;
	// it is necessary to read the file in binary mode in order to detect
	// them on Unix-based systems.
	//
	std::ifstream file(filePathStr, std::ios::in | std::ios::binary);
	if(!file.is_open())
	{
		return false;
	}

	PH_LOG(IesFile, "loading file <{}>", filePathStr);

	// TODO: consider using std::byte instead of char
	std::vector<char> buffer{std::istreambuf_iterator<char>(file), 
	                         std::istreambuf_iterator<char>()};

	if(!parse(buffer))
	{
		PH_LOG_WARNING(IesFile, "file <{}> parsing failed; file content read has {} bytes", 
			filePathStr, buffer.size());
		return false;
	}

	return true;
}

bool IesFile::parse(const std::vector<char>& data)
{
	// Although the maximum length of any label/keyword line (including the 
	// <CR><LF> terminating pair) is 82 characters, and the maximum length of 
	// any other line (including the <CR><LF> terminating pair) is 132 characters,
	// we do not limit the length of each line here. 
	//
	const std::vector<std::string> lines = retrieveLines(data);

	if(lines.empty())
	{
		PH_LOG_WARNING(IesFile, "no line detected in file <{}>", m_path.toAbsoluteString());
		return false;
	}

	std::size_t currentLine = 0;

	///////////////////////////////////////////////////////////////////////////
	// Start parsing the IES file.

	PH_ASSERT(currentLine == 0);

	// Line for file type.
	//
	if(currentLine < lines.size())
	{
		currentLine = parseFileType(lines, currentLine);
	}

	// Following is keywords (1991+) and labels (1986).
	//
	if(currentLine < lines.size())
	{
		currentLine = parseLabelsAndKeywords(lines, currentLine);
	}

	// The tilt line uniquely delimits the end of keyword/label lines.
	//
	if(currentLine < lines.size())
	{
		currentLine = parseTiltLine(lines, currentLine);
	}
	
	// There may be tilt data after the tilt line.
	//
	if(currentLine < lines.size())
	{
		currentLine = parseTiltData(lines, currentLine);
	}

	// First line of metadata for the light and later data.
	//
	if(currentLine < lines.size())
	{
		currentLine = parseMetadata1(lines, currentLine);
	}

	// Second line of metadata.
	//
	if(currentLine < lines.size())
	{
		currentLine = parseMetadata2(lines, currentLine);
	}

	// Vertical & horizontal angles for the photometric web.
	//
	if(currentLine < lines.size())
	{
		currentLine = parseAngles(lines, currentLine);
	}

	// Candela values on the photometric web.
	//
	if(currentLine < lines.size())
	{
		currentLine = parseCandelaValues(lines, currentLine);
	}

	PH_ASSERT(currentLine == lines.size());

	// Finish parsing the IES file.
	///////////////////////////////////////////////////////////////////////////

	return true;
}

/*
	IES LM-63-1991 and LM-63-1995 photometric data files begin with a unique
	file format identifier line, namely "IESNA91" or "IESNA:LM-63-1995".

	IES LM-63-1986 does not have a file format identifier line.
*/
std::size_t IesFile::parseFileType(const std::vector<std::string>& lines, const std::size_t currentLine)
{
	PH_ASSERT(currentLine < lines.size());

	const auto& line = lines[currentLine];
	if(line == "IESNA91")
	{
		m_iesFileType = EIesFileType::LM_63_1991;
		return currentLine + 1;
	}
	else if(line == "IESNA:LM-63-1995")
	{
		m_iesFileType = EIesFileType::LM_63_1995;
		return currentLine + 1;
	}
	else
	{
		m_iesFileType = EIesFileType::LM_63_1986;
		return currentLine;
	}
}

/*
	Label lines contain descriptive text about the luminaire, the lamp(s)
	used, and other descriptive comments.
*/
std::size_t IesFile::parseLabelsAndKeywords(const std::vector<std::string>& lines, const std::size_t currentLine)
{
	PH_ASSERT(currentLine < lines.size());

	// Label & keyword lines are before the tilt line.
	
	// find the index of line starting with "TILT="
	//
	std::size_t tiltLineIndex = lines.size();
	for(std::size_t i = currentLine; i < lines.size(); i++)
	{
		const std::string& line   = lines[i];
		const std::string& prefix = line.substr(0, 5);
		if(prefix == "TILT=")
		{
			tiltLineIndex = i;
			break;
		}
	}
	if(tiltLineIndex == lines.size())
	{
		PH_LOG_WARNING(IesFile, "no tilt line detected in file <{}>", 
			m_path.toAbsoluteString());
		return lines.size();
	}

	for(std::size_t i = currentLine; i < tiltLineIndex; i++)
	{
		const std::string& line = lines[i];

		// Keywords, which were introduced in LM-63-1991, require that each label
		// line begins with a defined IES keyword in square brackets.
		//
		if(line.length() > 0 && line[0] == '[')
		{
			const std::size_t tagEndIndex = line.find(']');
			const std::string tag         = line.substr(1, tagEndIndex - 1);
			const std::string data        = line.substr(tagEndIndex + 1);

			// required in LM-63-1991, but not in LM-63-1995
			//
			if(tag == "TEST")
			{
				m_testInfo += data;
			}
			// required in LM-63-1991, but not in LM-63-1995
			//
			else if(tag == "MANUFAC")
			{
				m_luminaireManufacturer += data;
			}
			else if(tag == "LUMCAT")
			{
				m_luminaireCatalogNumber += data;
			}
			else if(tag == "LUMINAIRE")
			{
				m_luminaireDescription += data;
			}
			else if(tag == "LAMPCAT")
			{
				m_lampCatalogNumber += data;
			}
			else if(tag == "LAMP")
			{
				m_lampDescription += data;
			}
			else
			{
				m_uncategorizedInfo += data;
			}
		}
		// IES LM-63-1986 does not require labels to be enclosed by brackets.
		//
		else
		{
			m_uncategorizedInfo += line;
		}
	}// end for each line

	return tiltLineIndex;
}

std::size_t IesFile::parseTiltLine(const std::vector<std::string>& lines, const std::size_t currentLine)
{
	PH_ASSERT(currentLine < lines.size());

	const std::string& line   = lines[currentLine];
	const std::string& prefix = line.substr(0, 5);
	if(prefix == "TILT=")
	{
		m_tilt = line.substr(5);
		if(m_tilt != "NONE")
		{
			PH_LOG_WARNING(IesFile, "for file <{}>, does not support tilt <{}>", 
				m_path.toAbsoluteString(), m_tilt);
			return lines.size();
		}

		return currentLine + 1;
	}
	else
	{
		PH_LOG_WARNING(IesFile, "invalid tilt line detected in file <{}>", 
			m_path.toAbsoluteString());
		return lines.size();
	}
}

std::size_t IesFile::parseTiltData(const std::vector<std::string>& lines, const std::size_t currentLine)
{
	PH_ASSERT(currentLine < lines.size());

	// Tilt data, if present, has 4 lines:
	//
	//  06  <lamp - to - luminaire geometry>
	//  07  <# of pairs of angles and multiplying factors>
	//  08  <angles>
	//  09  <multiplying factors>

	// If TILT=NONE, tilt data does not present in the photometric data file.
	//
	if(m_tilt == "NONE")
	{
		return currentLine;
	}
	// If TILT=INCLUDE, tilt data are in the photometric data file. Since we
	// do not support tilting, we simply ignore them.
	//
	else if(m_tilt == "INCLUDE")
	{
		return currentLine + 4;
	}
	// If TILT=<filename> (where "filename" is the name of a valid
	// TILT photometric data file), tilt data are present in the identified 
	// and separate TILT photometric data file. Since we do not support tilting, 
	// we ignore it again.
	//
	else
	{
		return currentLine;
	}
}

std::size_t IesFile::parseMetadata1(const std::vector<std::string>& lines, const std::size_t currentLine)
{
	static const Tokenizer tokenizer(std::vector<char>{' '});

	PH_ASSERT(currentLine < lines.size());

	std::vector<std::string> tokens;
	tokenizer.tokenize(lines[currentLine], tokens);
	if(tokens.size() != 10)
	{
		PH_LOG_WARNING(IesFile, 
			"the line for metadata 1 has {} tokens only (10 expected), will still try to parse (file {})", 
			tokens.size(), m_path.toAbsoluteString());
	}

	m_numLamps            = 0 < tokens.size() ? static_cast<uint32>(std::stoi(tokens[0])) : 1;
	m_lumensPerLamp       = 1 < tokens.size() ? static_cast<real>(std::stod(tokens[1]))   : 1000;
	m_candelaMultiplier   = 2 < tokens.size() ? static_cast<real>(std::stod(tokens[2]))   : 1;
	m_numVerticalAngles   = 3 < tokens.size() ? static_cast<uint32>(std::stoi(tokens[3])) : 0;
	m_numHorizontalAngles = 4 < tokens.size() ? static_cast<uint32>(std::stoi(tokens[4])) : 0;
	
	if(5 < tokens.size())
	{
		const int photometricWebTypeId = std::stoi(tokens[5]);
		switch(photometricWebTypeId)
		{
		case 1:  m_webType = EPhotometricWebType::C; break;
		case 2:  m_webType = EPhotometricWebType::B; break;
		case 3:  m_webType = EPhotometricWebType::A; break;
		default: m_webType = EPhotometricWebType::C; break;
		}
	}

	// Unit Type:
	// 1 = Feet
	// 2 = Meters
	//
	if(6 < tokens.size())
	{
		// TODO: currently ignored
	}

	// Luminous Opening Dimensions: width
	//
	if(7 < tokens.size())
	{
		// TODO: currently ignored
	}

	// Luminous Opening Dimensions: length
	//
	if(8 < tokens.size())
	{
		// TODO: currently ignored
	}

	// Luminous Opening Dimensions: height
	//
	if(9 < tokens.size())
	{
		// TODO: currently ignored
	}

	/*
		Nonrectangular Luminous Openings

		The luminous opening is normally considered to be rectangular. However,
		other predefined shapes can be modeled by specifying one or more of the
		above dimensions as zero or negative floating point numbers as follows:

		  Width  Length  Height  Description

			 0      0       0    Point
			 w      l       h    Rectangular (default)
			-d      0       0    Circular (where d = diameter of circle)
			-d      0      -d    Sphere (where d = diameter of circle)
			-d      0       h    Vertical cylinder (where d = diameter of
								 cylinder)
			 0      l      -d    Horizontal cylinder oriented along luminaire
								 length.
			 w      0      -d    Horizontal cylinder oriented along luminaire
								 width.
			-w      l       h    Ellipse oriented along luminaire length.
			 w     -l       h    Ellipse oriented along luminaire width.
			-w      l      -h    Ellipsoid oriented along luminaire length.
			 w     -l      -h    Ellipsoid oriented along luminaire width.

		See IES LM-63-1995 for detailed descriptions and diagrams.
	*/

	// TODO
	m_luminaireOpeningArea = 0.0_r;

	return currentLine + 1;
}

std::size_t IesFile::parseMetadata2(const std::vector<std::string>& lines, const std::size_t currentLine)
{
	static const Tokenizer tokenizer(std::vector<char>{' '});

	PH_ASSERT(currentLine < lines.size());

	std::vector<std::string> tokens;
	tokenizer.tokenize(lines[currentLine], tokens);
	if(tokens.size() != 3)
	{
		PH_LOG_WARNING(IesFile,
			"the line for metadata 2 has {} tokens only (3 expected), will still try to parse (file {})", 
			tokens.size(), m_path.toAbsoluteString());
	}

	m_ballastFactor                = 0 < tokens.size() ? static_cast<real>(std::stod(tokens[0])) : 1.0_r;
	m_ballastLampPhotometricFactor = 1 < tokens.size() ? static_cast<real>(std::stod(tokens[1])) : 1.0_r;
	m_inputWatts                   = 2 < tokens.size() ? static_cast<real>(std::stod(tokens[2])) : 100.0_r;

	return currentLine + 1;
}

std::size_t IesFile::parseAngles(const std::vector<std::string>& lines, const std::size_t currentLine)
{
	static const Tokenizer tokenizer(std::vector<char>{' '});

	PH_ASSERT(currentLine < lines.size());

	std::size_t parsingLine = currentLine;

	m_verticalAngles.clear();
	while(parsingLine < lines.size() && m_verticalAngles.size() != m_numVerticalAngles)
	{
		std::vector<std::string> tokens;
		tokenizer.tokenize(lines[parsingLine], tokens);
		for(const auto& token : tokens)
		{
			const double angle = std::stod(token);
			m_verticalAngles.push_back(static_cast<real>(angle));
		}
		parsingLine++;

		if(m_verticalAngles.size() > m_numVerticalAngles)
		{
			PH_LOG_WARNING(IesFile, "too many vertical angles in file {}", 
				m_path.toAbsoluteString());
			break;
		}
	}

	m_horizontalAngles.clear();
	while(parsingLine < lines.size() && m_horizontalAngles.size() != m_numHorizontalAngles)
	{
		std::vector<std::string> tokens;
		tokenizer.tokenize(lines[parsingLine], tokens);
		for(const auto& token : tokens)
		{
			const double angle = std::stod(token);
			m_horizontalAngles.push_back(static_cast<real>(angle));
		}
		parsingLine++;

		if(m_horizontalAngles.size() > m_numHorizontalAngles)
		{
			PH_LOG_WARNING(IesFile, "too many horizontal angles in file {}",
				m_path.toAbsoluteString());
			break;
		}
	}

	if(m_verticalAngles.size() != m_numVerticalAngles ||
	   m_horizontalAngles.size() != m_numHorizontalAngles)
	{
		PH_LOG_WARNING(IesFile, "mismatched number of angles in file {}",
			m_path.toAbsoluteString());
	}

	return parsingLine;
}

std::size_t IesFile::parseCandelaValues(const std::vector<std::string>& lines, const std::size_t currentLine)
{
	static const Tokenizer tokenizer(std::vector<char>{' '});

	PH_ASSERT(currentLine < lines.size());

	std::string joinedLines;
	for(std::size_t i = currentLine; i < lines.size(); i++)
	{
		joinedLines += lines[i];
		joinedLines += ' ';
	}

	std::vector<std::string> tokens;
	tokenizer.tokenize(joinedLines, tokens);

	const std::size_t expectedNumCandelaValues = m_numVerticalAngles * m_numHorizontalAngles;
	if(tokens.size() != expectedNumCandelaValues)
	{
		PH_LOG_WARNING(IesFile,
			"mismatched number of candela values ({}, expected to be {}) in file {}", 
			tokens.size(), expectedNumCandelaValues, m_path.toAbsoluteString());
	}

	m_candelaValues.clear();
	m_candelaValues.resize(m_numHorizontalAngles, std::vector<real>(m_numVerticalAngles, 0.0_r));
	for(std::size_t hIndex = 0; hIndex < m_numHorizontalAngles; hIndex++)
	{
		for(std::size_t vIndex = 0; vIndex < m_numVerticalAngles; vIndex++)
		{
			PH_ASSERT(hIndex < m_candelaValues.size() &&
			          vIndex < m_candelaValues[hIndex].size());

			const std::size_t tokenIndex = hIndex * m_numVerticalAngles + vIndex;
			const double value = tokenIndex < tokens.size() ? std::stod(tokens[tokenIndex]) : 0.0;
			m_candelaValues[hIndex][vIndex] = static_cast<real>(value);
		}
	}

	return lines.size();
}

std::string IesFile::getFilename() const
{
	return m_path.toAbsoluteString();
}

IesFile::EIesFileType IesFile::getIesFileType() const
{
	return m_iesFileType;
}

std::string IesFile::getTestInfo() const
{
	return m_testInfo;
}

std::string IesFile::getLuminaireManufacturer() const
{
	return m_luminaireManufacturer;
}

std::string IesFile::getLuminaireCatalogNumber() const
{
	return m_luminaireCatalogNumber;
}

std::string IesFile::getLuminaireDescription() const
{
	return m_luminaireDescription;
}

std::string IesFile::getLampCatalogNumber() const
{
	return m_lampCatalogNumber;
}

std::string IesFile::getLampDescription() const
{
	return m_lampDescription;
}

std::string IesFile::getUncategorizedInfo() const
{
	return m_uncategorizedInfo;
}

uint32 IesFile::getNumLamps() const
{
	return m_numLamps;
}

real IesFile::getLumensPerLamp() const
{
	return m_lumensPerLamp;
}

real IesFile::getCandelaMultiplier() const
{
	return m_candelaMultiplier;
}

uint32 IesFile::getNumVerticalAngles() const
{
	return m_numVerticalAngles;
}

uint32 IesFile::getNumHorizontalAngles() const
{
	return m_numHorizontalAngles;
}

IesFile::EPhotometricWebType IesFile::getPhotometricWebType() const
{
	return m_webType;
}

std::vector<real> IesFile::getVerticalAngles() const
{
	return m_verticalAngles;
}

std::vector<real> IesFile::getHorizontalAngles() const
{
	return m_horizontalAngles;
}

std::vector<std::vector<real>> IesFile::getCandelaValues() const
{
	return m_candelaValues;
}

/* 
	Split the data into lines of data based on IES's definition of 
	line separation (explicitly let <CR><LF> as line break).
*/
std::vector<std::string> IesFile::retrieveLines(const std::vector<char>& data)
{
	std::size_t lineBeginIndex = 0;
	std::size_t lineEndIndex   = 0;
	std::vector<std::string> lines;
	while(lineEndIndex < data.size())
	{
		const char thisChar = data[lineEndIndex];
		const char nextChar = (lineEndIndex + 1) < data.size() ? data[lineEndIndex + 1] : '\0';
		if(thisChar == '\r' && nextChar == '\n')
		{
			std::string line;
			for(std::size_t i = lineBeginIndex; i < lineEndIndex; i++)
			{
				line += data[i];
			}
			lines.push_back(line);

			lineEndIndex += 2;
			lineBeginIndex = lineEndIndex;
		}
		else
		{
			lineEndIndex++;
		}
	}

	return lines;
}

}// end namespace ph
