#include "DataIO/PlyFile.h"
#include "Common/logging.h"
#include "DataIO/Stream/BinaryFileInputStream.h"
#include "DataIO/io_exceptions.h"
#include "Utility/string_utils.h"

namespace ph
{

PH_DEFINE_INTERNAL_LOG_GROUP(PlyFile, DataIO);

namespace
{

enum class EPlyHeaderEntry
{
	UNSPECIFIED = 0,
	Property,
	Element,
	Comment,
	Format
};

inline std::string_view format_to_ply_keyword(const EPlyFileFormat format)
{
	switch(format)
	{
	case EPlyFileFormat::Binary:          return "binary_little_endian";
	case EPlyFileFormat::ASCII:           return "ascii";
	case EPlyFileFormat::BinaryBigEndian: return "binary_big_endian";
	default: 
		PH_LOG_WARNING(PlyFile, "Unknown PLY format, cannot convert to keyword.");
		return "";
	}
}

inline EPlyFileFormat ply_keyword_to_format(const std::string_view keyword)
{
	if(keyword == format_to_ply_keyword(EPlyFileFormat::Binary))
	{
		return EPlyFileFormat::Binary;
	}
	else if(keyword == format_to_ply_keyword(EPlyFileFormat::ASCII))
	{
		return EPlyFileFormat::ASCII;
	}
	else if(keyword == format_to_ply_keyword(EPlyFileFormat::BinaryBigEndian))
	{
		return EPlyFileFormat::BinaryBigEndian;
	}
	else
	{
		PH_LOG_WARNING(PlyFile, "Unknown PLY keyword: {}, cannot identify format; assuming ASCII.", keyword);
		return EPlyFileFormat::ASCII;
	}
}

inline std::string_view entry_to_ply_keyword(const EPlyHeaderEntry entry)
{
	switch(entry)
	{
	case EPlyHeaderEntry::Property: return "property";
	case EPlyHeaderEntry::Element:  return "element";
	case EPlyHeaderEntry::Comment:  return "comment";
	case EPlyHeaderEntry::Format:   return "format";
	default: 
		PH_LOG_WARNING(PlyFile, "Unknown PLY entry, cannot convert to keyword.");
		return "";
	}
}

inline EPlyHeaderEntry ply_keyword_to_entry(const std::string_view keyword)
{
	if(keyword == entry_to_ply_keyword(EPlyHeaderEntry::Property))
	{
		return EPlyHeaderEntry::Property;
	}
	else if(keyword == entry_to_ply_keyword(EPlyHeaderEntry::Element))
	{
		return EPlyHeaderEntry::Element;
	}
	else if(keyword == entry_to_ply_keyword(EPlyHeaderEntry::Comment))
	{
		return EPlyHeaderEntry::Comment;
	}
	else if(keyword == entry_to_ply_keyword(EPlyHeaderEntry::Format))
	{
		return EPlyHeaderEntry::Format;
	}
	else
	{
		PH_LOG_WARNING(PlyFile, "Unknown PLY keyword: {}, cannot identify entry.", keyword);
		return EPlyHeaderEntry::UNSPECIFIED;
	}
}

}// end anonymous namespace

PlyFile::PlyProperty::PlyProperty() :
	name         (),
	dataType     (EPlyDataType::UNSPECIFIED),
	listCountType(EPlyDataType::UNSPECIFIED)
{}

bool PlyFile::PlyProperty::isList() const
{
	return listCountType != EPlyDataType::UNSPECIFIED;
}

PlyFile::PlyElement::PlyElement() :
	name       (),
	numElements(0),
	properties (),
	rawBuffer  ()
{}

PlyFile::PlyFile() :
	m_format  (EPlyFileFormat::ASCII),
	m_version (),
	m_comments(),
	m_elements()
{}

PlyFile::PlyFile(const Path& plyFilePath) :
	PlyFile(plyFilePath, PlyIOConfig())
{}

PlyFile::PlyFile(const Path& plyFilePath, const PlyIOConfig& config) :
	PlyFile()
{

}

void PlyFile::setFormat(const EPlyFileFormat format)
{
	m_format = format;
}

void PlyFile::clear()
{
	m_comments.clear();
	m_elements.clear();
}

SemanticVersion PlyFile::getVersion() const
{
	return m_version;
}

void PlyFile::readHeader(std::string_view headerStr, const PlyIOConfig& config, const Path& plyFilePath)
{
	// Check for valid header begin/end guards, then remove them
	
	constexpr std::string_view MAGIC_NUMBER = "ply\r";
	constexpr std::string_view HEADER_END   = "end_header\r";

	if(!headerStr.starts_with(MAGIC_NUMBER))
	{
		throw FileIOError("Invalid PLY file magic number", plyFilePath.toAbsoluteString());
	}
	headerStr.remove_prefix(MAGIC_NUMBER.size());

	if(!headerStr.ends_with(HEADER_END))
	{
		throw FileIOError("Invalid PLY file header ending", plyFilePath.toAbsoluteString());
	}
	headerStr.remove_suffix(HEADER_END.size());

	// Parse line by line (\r terminated) and populate definition of elements
	// (as well as attributes other than raw data, e.g., file format and comments)

	while(true)
	{
		// Remove a line from the header

		const auto carriageReturnIndex = headerStr.find('\r');
		if(carriageReturnIndex == std::string_view::npos)
		{
			break;
		}

		auto headerLine = string_utils::trim(headerStr.substr(0, carriageReturnIndex));
		headerStr = headerStr.substr(carriageReturnIndex + 1);
		if(headerLine.empty())
		{
			continue;
		}

		// Detect and remove an entry from the line

		auto entry = EPlyHeaderEntry::UNSPECIFIED;
		if(auto spacePos = headerLine.find(' '); spacePos != std::string_view::npos)
		{
			entry = ply_keyword_to_entry(headerLine.substr(0, spacePos));
			headerLine = string_utils::trim(headerLine.substr(spacePos + 1));
		}
		else
		{
			throw FileIOError("PLY header line with unknown entry", plyFilePath.toAbsoluteString());
		}

		switch(entry)
		{
		case EPlyHeaderEntry::Property:
			if(headerLine.starts_with(KEYWORD_PROPERTY))
			{
				// TODO

				hasParsedLine = true;
			}
			break;

		case EPlyHeaderEntry::Element:
			if(headerLine.starts_with(KEYWORD_ELEMENT))
			{
				// TODO

				hasParsedLine = true;
			}
			break;

		case KEYWORD_COMMENT.front():
			if(headerLine.starts_with(KEYWORD_COMMENT))
			{
				if(!config.bIgnoreComments)
				{
					// TODO
				}

				hasParsedLine = true;
			}
			break;

		case KEYWORD_FORMAT.front():
			if(headerLine.starts_with(KEYWORD_FORMAT))
			{
				headerLine.remove_prefix(KEYWORD_FORMAT.size());
				headerLine = string_utils::trim(headerLine);

				if(auto spacePos = headerLine.find(' '); spacePos != std::string_view::npos)
				{
					m_format = ply_keyword_to_format(headerLine.substr(0, spacePos));
					headerLine = headerLine.substr(spacePos + 1);
				}
				else
				{

				}

				if(!headerLine.empty())
				{
					m_version = SemanticVersion(string_utils::trim(headerLine));
				}

				hasParsedLine = true;
			}
			break;

		}

		if(!hasParsedLine)
		{
			PH_LOG_WARNING(PlyFile, "Unknown header information: {}, ignoring", headerLine);
		}
	}// end while each header line
	
}

}// end namespace ph
