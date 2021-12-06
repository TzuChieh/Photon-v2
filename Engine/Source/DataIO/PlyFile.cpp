#include "DataIO/PlyFile.h"
#include "Common/logging.h"
#include "DataIO/Stream/BinaryFileInputStream.h"
#include "DataIO/Stream/ByteBufferInputStream.h"
#include "DataIO/io_exceptions.h"
#include "Utility/string_utils.h"

#include <type_traits>
#include <memory>
#include <utility>
#include <cstring>
#include <stdexcept>

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

	for(Value ei = static_cast<Value>(EPlyHeaderEntry::UNSPECIFIED) + 1; 
	    ei < static_cast<Value>(EPlyHeaderEntry::NUM); 
	    ++ei)
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

	// We could use a loop to find matching type just like ply_keyword_to_entry(); however, some PLY files
	// have non-standard data type keywords. As a result, we manually test them.

	const char firstChar = keyword.empty() ? '\0' : keyword.front();
	switch(firstChar)
	{
	case 'c':
		if(keyword == "char")
		{
			return EPlyDataType::PPT_int8;
		}
		break;

	case 'd':
		if(keyword == "double")
		{
			return EPlyDataType::PPT_float64;
		}
		break;

	case 'f':
		if(keyword == "float" || keyword == "float32")
		{
			return EPlyDataType::PPT_float32;
		}
		else if(keyword == "float64")
		{
			return EPlyDataType::PPT_float64;
		}
		break;

	case 'i':
		if(keyword == "int" || keyword == "int32")
		{
			return EPlyDataType::PPT_int32;
		}
		else if(keyword == "int8")
		{
			return EPlyDataType::PPT_int8;
		}
		else if(keyword == "int16")
		{
			return EPlyDataType::PPT_int16;
		}
		break;

	case 's':
		if(keyword == "short")
		{
			return EPlyDataType::PPT_int16;
		}
		break;

	case 'u':
		if(keyword == "uchar" || keyword == "uint8")
		{
			return EPlyDataType::PPT_uint8;
		}
		else if(keyword == "ushort" || keyword == "uint16")
		{
			return EPlyDataType::PPT_uint16;
		}
		else if(keyword == "uint" || keyword == "uint32")
		{
			return EPlyDataType::PPT_uint32;
		}
		break;
	}

	PH_LOG_WARNING(PlyFile, "Unknown PLY keyword: {}, cannot identify data type.", keyword);
	return EPlyDataType::UNSPECIFIED;
}

inline std::size_t sizeof_ply_data_type(const EPlyDataType dataType)
{
	switch(dataType)
	{
	case EPlyDataType::PPT_int8:
	case EPlyDataType::PPT_uint8:
		return 1;

	case EPlyDataType::PPT_int16:
	case EPlyDataType::PPT_uint16:
		return 2;

	case EPlyDataType::PPT_int32:
	case EPlyDataType::PPT_uint32:
	case EPlyDataType::PPT_float32:
		return 4;

	case EPlyDataType::PPT_float64:
		return 8;
	}

	PH_ASSERT_UNREACHABLE_SECTION();
	return 0;
}

template<typename DataType>
inline float64 ascii_ply_data_to_binary(const std::string_view asciiPlyData, std::byte* const out_bytes)
{
	PH_ASSERT(out_bytes);

	const auto value = string_utils::parse_number<DataType>(asciiPlyData);
	std::memcpy(out_bytes, &value, sizeof(DataType));
	return static_cast<float64>(value);
}

inline float64 ascii_ply_data_to_binary(
	const std::string_view asciiPlyData, 
	const EPlyDataType     dataType, 
	std::byte* const       out_bytes)
{
	switch(dataType)
	{
	case EPlyDataType::PPT_int8:
		return ascii_ply_data_to_binary<int8>(asciiPlyData, out_bytes);

	case EPlyDataType::PPT_uint8:
		return ascii_ply_data_to_binary<uint8>(asciiPlyData, out_bytes);

	case EPlyDataType::PPT_int16:
		return ascii_ply_data_to_binary<int16>(asciiPlyData, out_bytes);

	case EPlyDataType::PPT_uint16:
		return ascii_ply_data_to_binary<uint16>(asciiPlyData, out_bytes);

	case EPlyDataType::PPT_int32:
		return ascii_ply_data_to_binary<int32>(asciiPlyData, out_bytes);

	case EPlyDataType::PPT_uint32:
		return ascii_ply_data_to_binary<uint32>(asciiPlyData, out_bytes);

	case EPlyDataType::PPT_float32:
		return ascii_ply_data_to_binary<float32>(asciiPlyData, out_bytes);

	case EPlyDataType::PPT_float64:
		return ascii_ply_data_to_binary<float64>(asciiPlyData, out_bytes);
	}

	PH_ASSERT_UNREACHABLE_SECTION();
	return 0.0;
}

}// end anonymous namespace

PlyProperty::PlyProperty() :
	name         (),
	dataType     (EPlyDataType::UNSPECIFIED),
	listSizeType (EPlyDataType::UNSPECIFIED),
	fixedListSize(0)
{}

bool PlyProperty::isList() const
{
	return listSizeType != EPlyDataType::UNSPECIFIED;
}

bool PlyProperty::isFixedSizeList() const
{
	return fixedListSize > 0;
}

PlyElement::PlyElement() :
	name       (),
	numElements(0),
	properties (),
	rawBuffer  ()
{}

bool PlyElement::isLoaded() const
{
	return !rawBuffer.empty();
}

std::size_t PlyElement::estimateStrideSize() const
{
	std::size_t estimatedStrideSize = 0;
	for(const PlyProperty& prop : properties)
	{
		if(prop.isList())
		{
			const auto listSize = prop.isFixedSizeList() 
				? prop.fixedListSize
				: 3;// assumed 3 as in most files lists are for triangle vertices

			estimatedStrideSize += listSize * sizeof_ply_data_type(prop.dataType);
			estimatedStrideSize += sizeof_ply_data_type(prop.listSizeType);
		}
		else
		{
			estimatedStrideSize += sizeof_ply_data_type(prop.dataType);
		}
	}

	return estimatedStrideSize * numElements;
}

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
	clearBuffer();
	loadFile(plyFilePath, config);
}

const PlyElement* PlyFile::findElement(const std::string_view name) const
{
	for(const PlyElement& element : m_elements)
	{
		if(element.name == name)
		{
			return &element;
		}
	}
	return nullptr;
}

std::size_t PlyFile::numElements() const
{
	return m_elements.size();
}

EPlyFileFormat PlyFile::getFormat() const
{
	return m_format;
}

std::size_t PlyFile::numComments() const
{
	return m_comments.size();
}

std::string_view PlyFile::getComment(const std::size_t commentIndex) const
{
	PH_ASSERT_LT(commentIndex, m_comments.size());

	if(commentIndex < m_comments.size())
	{
		return m_comments[commentIndex];
	}
	else
	{
		return "";
	}
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

void PlyFile::reserveBuffer()
{
	for(PlyElement& element : m_elements)
	{
		// Skip already loaded buffer
		if(element.isLoaded())
		{
			continue;
		}

		// Reserve memory space for the element buffer
		element.rawBuffer.reserve(element.estimateStrideSize());
	}
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
			fileStream->read(*fileSize, preloadedStream->byteBuffer());

			stream = std::move(preloadedStream);
		}
		else
		{
			stream = std::move(fileStream);
		}

		shouldReduceStorageMemory = fileSize && *fileSize > config.reduceStorageMemoryThreshold;
	}
	
	// Load PLY header

	parseHeader(*stream, config, plyFilePath);

	// Load PLY buffer

	reserveBuffer();

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
					prop.listSizeType = ply_keyword_to_data_type(next_token(headerLine, &headerLine));
					prop.dataType     = ply_keyword_to_data_type(next_token(headerLine, &headerLine));
					prop.name         = next_token(headerLine);
				}
				else
				{
					prop.dataType = ply_keyword_to_data_type(tokenAfterEntry);
					prop.name     = next_token(headerLine);
				}

				PH_ASSERT(!m_elements.empty());
				m_elements.back().properties.push_back(prop);
			}
			break;

		case EPlyHeaderEntry::Element:
			m_elements.push_back(PlyElement());
			m_elements.back().name = next_token(headerLine, &headerLine);
			m_elements.back().numElements = parse_int<std::size_t>(next_token(headerLine));
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
			m_version = SemanticVersion(next_token(headerLine));
			break;

		default:
			PH_ASSERT_UNREACHABLE_SECTION();
			break;
		}
	}// end while each header line
}

void PlyFile::loadTextBuffer(IInputStream& stream, const PlyIOConfig& config, const Path& plyFilePath)
{
	using namespace string_utils;

	std::string lineBuffer;
	lineBuffer.reserve(128);

	for(PlyElement& element : m_elements)
	{
		// Skip already loaded buffer
		if(element.isLoaded())
		{
			continue;
		}

		std::vector<std::byte>& rawBuffer = element.rawBuffer;

		try
		{
			for(std::size_t ei = 0; ei < element.numElements; ++ei)
			{
				// Each line describes a single element
				stream.readLine(&lineBuffer);
				auto currentLine = trim(lineBuffer);

				for(PlyProperty& prop : element.properties)
				{
					if(prop.isList())
					{
						const std::size_t firstSizeByteIdx = rawBuffer.size();
						rawBuffer.resize(rawBuffer.size() + sizeof_ply_data_type(prop.listSizeType));

						const auto listSize = static_cast<std::size_t>(ascii_ply_data_to_binary(
							next_token(currentLine, &currentLine),
							prop.dataType,
							&(rawBuffer[firstSizeByteIdx])));

						const std::size_t firstByteIdx = rawBuffer.size();
						const std::size_t sizeofData   = sizeof_ply_data_type(prop.dataType);
						rawBuffer.resize(rawBuffer.size() + sizeofData * listSize);
						for(std::size_t li = 0; li < listSize; ++li)
						{
							ascii_ply_data_to_binary(
								next_token(currentLine, &currentLine), 
								prop.dataType, 
								&(rawBuffer[firstByteIdx + li * sizeofData]));
						}

						// Only set list size on the first encounter. Later if there is a disagreement of 
						// list size, the list must be variable-sized and we set the size to 0. Also handles
						// the case where <prop.fixedListSize> is already set to 0.
						PH_ASSERT_GT(listSize, 0);
						if(prop.fixedListSize != listSize)
						{
							prop.fixedListSize = ei == 0 ? listSize : 0;
						}
					}
					else
					{
						const std::size_t firstByteIdx = rawBuffer.size();
						rawBuffer.resize(rawBuffer.size() + sizeof_ply_data_type(prop.dataType));

						ascii_ply_data_to_binary(
							next_token(currentLine, &currentLine), 
							prop.dataType, 
							&(rawBuffer[firstByteIdx]));
					}
				}
			}
		}
		catch(const std::runtime_error& e)
		{
			throw FileIOError(std::format(
				"Error loading value from element {}, detail: {}", element.name, e.what()), 
				plyFilePath.toAbsoluteString());
		}
	}
}

void PlyFile::loadBinaryBuffer(IInputStream& stream, const PlyIOConfig& config, const Path& plyFilePath)
{

}

}// end namespace ph
