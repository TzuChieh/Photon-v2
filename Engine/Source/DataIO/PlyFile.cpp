#include "DataIO/PlyFile.h"
#include "Common/logging.h"
#include "DataIO/Stream/BinaryFileInputStream.h"
#include "DataIO/Stream/ByteBufferInputStream.h"
#include "DataIO/io_exceptions.h"
#include "Utility/string_utils.h"

#include <type_traits>
#include <memory>
#include <utility>

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
	loadFile(plyFilePath, config);
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

void PlyFile::compactBuffer()
{
	m_comments.shrink_to_fit();

	for(auto& element : m_elements)
	{
		element.properties.shrink_to_fit();
		element.rawBuffer.shrink_to_fit();
	}
	m_elements.shrink_to_fit();
}

SemanticVersion PlyFile::getVersion() const
{
	return m_version;
}

void PlyFile::loadFile(const Path& plyFilePath, const PlyIOConfig& config)
{
	std::unique_ptr<IInputStream> stream;
	bool shouldReduceStorageMemory = false;
	{
		auto fileStream = std::make_unique<BinaryFileInputStream>(plyFilePath);

		// Possibly preload file data into memory. Preloading might vastly increase parsing performance for 
		// smaller files.
		const std::optional<std::size_t> fileSize = fileStream->getFileSizeInBytes();
		if(fileSize && *fileSize < config.preloadMemoryThreshold)
		{
			auto preloadedStream = std::make_unique<ByteBufferInputStream>(*fileSize);
			stream->read(*fileSize, preloadedStream->byteBuffer());

			stream = std::move(preloadedStream);
		}
		else
		{
			stream = std::move(fileStream);
		}

		shouldReduceStorageMemory = fileSize && *fileSize > config.reduceStorageMemoryThreshold;
	}
	
	parseHeader(*stream, config, plyFilePath);

	if(m_format == EPlyFileFormat::ASCII)
	{
		loadTextBuffer(*stream, config, plyFilePath);
	}
	else
	{
		loadBinaryBuffer(*stream, config, plyFilePath);
	}

	if(shouldReduceStorageMemory)
	{
		compactBuffer();
	}
}

void PlyFile::loadTextBuffer(IInputStream& stream, const PlyIOConfig& config, const Path& plyFilePath)
{

}

void PlyFile::loadBinaryBuffer(IInputStream& stream, const PlyIOConfig& config, const Path& plyFilePath)
{

}

void PlyFile::parseHeader(IInputStream& stream, const PlyIOConfig& config, const Path& plyFilePath)
{
	using namespace string_utils;

	// Header guards, marks the start & end of the header block
	constexpr std::string_view MAGIC_NUMBER = "ply";
	constexpr std::string_view HEADER_END   = "end_header";

	std::string lineBuffer;
	lineBuffer.reserve(128);

	// Note that a popular PLY spec (http://paulbourke.net/dataformats/ply/) says that each header line is
	// terminated by a carriage return; however, for the files we can find they are terminated by line feed.
	// We assume line termination by line feed in the following implementation.

	stream.readLine(&lineBuffer);
	if(trim(lineBuffer) != MAGIC_NUMBER)
	{
		throw FileIOError("Invalid PLY file magic number", plyFilePath.toAbsoluteString());
	}

	// Parse line by line and populate definition of elements (as well as attributes other than raw data, 
	// e.g., file format and comments)

	while(true)
	{
		stream.readLine(&lineBuffer);

		// Trimming the line should unify the use of LF and CRLF (eliminating the extra CR after readLine())
		auto headerLine = trim(lineBuffer);
		if(headerLine.empty())
		{
			continue;
		}

		if(headerLine == HEADER_END)
		{
			break;
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
