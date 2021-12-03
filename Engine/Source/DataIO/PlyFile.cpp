#include "DataIO/PlyFile.h"
#include "Common/logging.h"
#include "DataIO/Stream/BinaryFileInputStream.h"
#include "DataIO/io_exceptions.h"
#include "Utility/string_utils.h"

#include <type_traits>

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
	Format,

	NUM
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
	using Value = std::underlying_type_t<EPlyFileFormat>;

	for(Value ei = 0; ei < static_cast<Value>(EPlyFileFormat::NUM); ++ei)
	{
		const auto enumValue = static_cast<EPlyFileFormat>(ei);
		if(keyword == format_to_ply_keyword(enumValue))
		{
			return enumValue;
		}
	}

	PH_LOG_WARNING(PlyFile, "Unknown PLY keyword: {}, cannot identify format; assuming ASCII.", keyword);
	return EPlyFileFormat::ASCII;
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
	using Value = std::underlying_type_t<EPlyHeaderEntry>;

	for(Value ei = 0; ei < static_cast<Value>(EPlyHeaderEntry::NUM); ++ei)
	{
		const auto enumValue = static_cast<EPlyHeaderEntry>(ei);
		if(keyword == entry_to_ply_keyword(enumValue))
		{
			return enumValue;
		}
	}

	PH_LOG_WARNING(PlyFile, "Unknown PLY keyword: {}, cannot identify entry.", keyword);
	return EPlyHeaderEntry::UNSPECIFIED;
}

inline std::string_view data_type_to_ply_keyword(const EPlyDataType dataType)
{
	switch(dataType)
	{
	case EPlyDataType::PPT_int8:    return "char";
	case EPlyDataType::PPT_uint8:   return "uchar";
	case EPlyDataType::PPT_int16:   return "short";
	case EPlyDataType::PPT_uint16:  return "ushort";
	case EPlyDataType::PPT_int32:   return "int";
	case EPlyDataType::PPT_uint32:  return "uint";
	case EPlyDataType::PPT_float32: return "float";
	case EPlyDataType::PPT_float64: return "double";
	default: 
		PH_LOG_WARNING(PlyFile, "Unknown PLY data type, cannot convert to keyword.");
		return "";
	}
}

inline EPlyDataType ply_keyword_to_data_type(const std::string_view keyword)
{
	using Value = std::underlying_type_t<EPlyDataType>;

	for(Value ei = 0; ei < static_cast<Value>(EPlyDataType::NUM); ++ei)
	{
		const auto enumValue = static_cast<EPlyDataType>(ei);
		if(keyword == data_type_to_ply_keyword(enumValue))
		{
			return enumValue;
		}
	}

	PH_LOG_WARNING(PlyFile, "Unknown PLY keyword: {}, cannot identify data type.", keyword);
	return EPlyDataType::UNSPECIFIED;
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
	m_version (1, 0, 0),
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

void PlyFile::clearBuffer()
{
	m_comments.clear();
	m_elements.clear();
}

SemanticVersion PlyFile::getVersion() const
{
	return m_version;
}

void PlyFile::parseHeader(std::string_view headerStr, const PlyIOConfig& config, const Path& plyFilePath)
{
	using namespace string_utils;

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

		auto headerLine = trim(headerStr.substr(0, carriageReturnIndex));
		headerStr = headerStr.substr(carriageReturnIndex + 1);
		if(headerLine.empty())
		{
			continue;
		}

		// Detect and remove the entry keyword from the line

		const auto entry = ply_keyword_to_entry(next_token(headerLine, &headerLine));
		if(entry == EPlyHeaderEntry::UNSPECIFIED)
		{
			throw FileIOError("PLY header line with unknown entry", plyFilePath.toAbsoluteString());
		}

		switch(entry)
		{
		case EPlyHeaderEntry::Property:
			if(m_elements.empty())
			{
				throw FileIOError("PLY header defines a property without element", plyFilePath.toAbsoluteString());
			}

			{
				PlyProperty prop;

				const auto tokenAfterEntry = next_token(headerLine, &headerLine);
				if(tokenAfterEntry == "list")
				{
					prop.listCountType = ply_keyword_to_data_type(next_token(headerLine, &headerLine));
					prop.dataType      = ply_keyword_to_data_type(next_token(headerLine, &headerLine));
					prop.name          = headerLine;
				}
				else
				{
					prop.dataType = ply_keyword_to_data_type(next_token(headerLine, &headerLine));
					prop.name     = headerLine;
				}

				PH_ASSERT(!m_elements.empty());
				m_elements.back().properties.push_back(prop);
			}
			break;

		case EPlyHeaderEntry::Element:
			m_elements.push_back(PlyElement());
			m_elements.back().name = next_token(headerLine, &headerLine);
			m_elements.back().numElements = parse_int<std::size_t>(headerLine);
			break;

		case EPlyHeaderEntry::Comment:
			if(!config.bIgnoreComments)
			{
				// The rest of the line should all be comments
				m_comments.push_back(std::string(headerLine));
			}
			break;

		case EPlyHeaderEntry::Format:
			m_format  = ply_keyword_to_format(next_token(headerLine, &headerLine));
			m_version = SemanticVersion(headerLine);
			break;

		default:
			PH_ASSERT_UNREACHABLE_SECTION();
			break;
		}
	}// end while each header line
}

}// end namespace ph
