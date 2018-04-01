#include "FileIO/Data/IesFile.h"
#include "Common/assertion.h"

#include <fstream>
#include <vector>
#include <string>

namespace ph
{

const Logger IesFile::logger(LogSender("IES File"));

IesFile::IesFile(const Path& path) : 
	m_path(path), 
	m_iesFileType(EIesFileType::UNKNOWN)
{}

bool IesFile::load()
{
	const std::string filePathStr = m_path.toAbsoluteString();

	// In an IES file, all lines shall be terminated with a <CR><LF> pair;
	// it is necessary to read the file in binary mode in order to detect
	// them on Unix-based systems.
	//
	std::ifstream file(filePathStr, std::ios::binary);
	if(!file.is_open())
	{
		return false;
	}

	logger.log(ELogLevel::NOTE_MED, 
		"loading file <" + filePathStr + ">");

	std::vector<char> buffer{std::istreambuf_iterator<char>(file), 
	                         std::istreambuf_iterator<char>()};
	if(!parse(buffer))
	{
		logger.log(ELogLevel::WARNING_MED,
			"file <" + filePathStr + "> parsing failed");
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
		logger.log(ELogLevel::WARNING_MED,
			"no line detected in file <" + m_path.toAbsoluteString() + ">");
		return false;
	}

	std::size_t currentLine = 0;

	if(currentLine < lines.size())
	{
		currentLine = parseFileType(lines, currentLine);
	}

	if(currentLine < lines.size())
	{
		currentLine = parseLabelsAndKeywords(lines, currentLine);
	}

	if(currentLine < lines.size())
	{
		currentLine = parseTiltLine(lines, currentLine);
	}
	
	if(currentLine < lines.size())
	{
		currentLine = parseTiltData(lines, currentLine);
	}

	// TODO

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
		logger.log(ELogLevel::WARNING_MED,
			"no tilt line detected in file <" + m_path.toAbsoluteString() + ">");
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
			logger.log(ELogLevel::WARNING_MED,
				"for file <" + m_path.toAbsoluteString() + ">, " + 
				"does not support tilt <" + m_tilt + ">");
			return lines.size();
		}

		return currentLine + 1;
	}
	else
	{
		logger.log(ELogLevel::WARNING_MED,
			"invalid tilt line detected in file <" + m_path.toAbsoluteString() + ">");
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

	/*int i = 0;
	for(const auto& ch : data)
	{
		std::cerr << std::hex << static_cast<int>(ch) << " ";
		i++;
		if(i == 16)
		{
			i = 0;
		}
	}*/

	//std::ofstream file("./test.txt");
	//for(const auto& line : lines)
	//{
	//	//std::cerr << line << std::endl;
	//	file << line;
	//}

	return lines;
}

}// end namespace ph