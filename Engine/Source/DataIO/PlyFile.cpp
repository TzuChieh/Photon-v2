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
#include <array>

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
inline DataType read_binary_ply_data(const std::byte* const binaryPlyData)
{
	static_assert(std::is_trivially_copyable_v<DataType>);
	PH_ASSERT(binaryPlyData);

	DataType value;
	std::memcpy(&value, binaryPlyData, sizeof(DataType));
	return value;
}

template<typename DataType>
inline void write_binary_ply_data(const DataType value, std::byte* const out_binaryPlyData)
{
	static_assert(std::is_trivially_copyable_v<DataType>);
	PH_ASSERT(out_binaryPlyData);

	std::memcpy(out_binaryPlyData, &value, sizeof(DataType));
}

template<typename DataType>
inline float64 ascii_ply_data_to_bytes(const std::string_view asciiPlyData, std::byte* const out_bytes)
{
	const auto value = string_utils::parse_number<DataType>(asciiPlyData);
	write_binary_ply_data<DataType>(value, out_bytes);

	return static_cast<float64>(value);
}

template<typename DataType>
inline void bytes_to_ascii_ply_data(const std::byte* const binaryPlyData, std::string* const out_asciiPlyData)
{
	const auto value = read_binary_ply_data<DataType>(binaryPlyData);
	string_utils::stringify_number<DataType>(value, out_asciiPlyData);
}

inline float64 bytes_to_ply_data(const std::byte* const binaryPlyData, const EPlyDataType dataType)
{
	switch(dataType)
	{
	case EPlyDataType::PPT_int8:    return read_binary_ply_data<int8>(binaryPlyData);
	case EPlyDataType::PPT_uint8:   return read_binary_ply_data<uint8>(binaryPlyData);
	case EPlyDataType::PPT_int16:   return read_binary_ply_data<int16>(binaryPlyData);
	case EPlyDataType::PPT_uint16:  return read_binary_ply_data<uint16>(binaryPlyData);
	case EPlyDataType::PPT_int32:   return read_binary_ply_data<int32>(binaryPlyData);
	case EPlyDataType::PPT_uint32:  return read_binary_ply_data<uint32>(binaryPlyData);
	case EPlyDataType::PPT_float32: return read_binary_ply_data<float32>(binaryPlyData);
	case EPlyDataType::PPT_float64: return read_binary_ply_data<float64>(binaryPlyData);
	}

	PH_ASSERT_UNREACHABLE_SECTION();
	return 0.0;
}

inline void ply_data_to_bytes(const float64 value, const EPlyDataType dataType, std::byte* const out_binaryPlyData)
{
	switch(dataType)
	{
	case EPlyDataType::PPT_int8: 
		write_binary_ply_data<int8>(value, out_binaryPlyData);
		break;

	case EPlyDataType::PPT_uint8:
		write_binary_ply_data<uint8>(value, out_binaryPlyData);
		break;

	case EPlyDataType::PPT_int16:
		write_binary_ply_data<int16>(value, out_binaryPlyData);
		break;

	case EPlyDataType::PPT_uint16: 
		write_binary_ply_data<uint16>(value, out_binaryPlyData);
		break;

	case EPlyDataType::PPT_int32:
		write_binary_ply_data<int32>(value, out_binaryPlyData);
		break;

	case EPlyDataType::PPT_uint32: 
		write_binary_ply_data<uint32>(value, out_binaryPlyData);
		break;

	case EPlyDataType::PPT_float32:
		write_binary_ply_data<float32>(value, out_binaryPlyData);
		break;

	case EPlyDataType::PPT_float64:
		write_binary_ply_data<float64>(value, out_binaryPlyData);
		break;
	}

	PH_ASSERT_UNREACHABLE_SECTION();
}

inline float64 ascii_ply_data_to_bytes(
	const std::string_view asciiPlyData, 
	const EPlyDataType     dataType, 
	std::byte* const       out_binaryPlyData)
{
	switch(dataType)
	{
	case EPlyDataType::PPT_int8:    return ascii_ply_data_to_bytes<int8>(asciiPlyData, out_binaryPlyData);
	case EPlyDataType::PPT_uint8:   return ascii_ply_data_to_bytes<uint8>(asciiPlyData, out_binaryPlyData);
	case EPlyDataType::PPT_int16:   return ascii_ply_data_to_bytes<int16>(asciiPlyData, out_binaryPlyData);
	case EPlyDataType::PPT_uint16:  return ascii_ply_data_to_bytes<uint16>(asciiPlyData, out_binaryPlyData);
	case EPlyDataType::PPT_int32:   return ascii_ply_data_to_bytes<int32>(asciiPlyData, out_binaryPlyData);
	case EPlyDataType::PPT_uint32:  return ascii_ply_data_to_bytes<uint32>(asciiPlyData, out_binaryPlyData);
	case EPlyDataType::PPT_float32: return ascii_ply_data_to_bytes<float32>(asciiPlyData, out_binaryPlyData);
	case EPlyDataType::PPT_float64: return ascii_ply_data_to_bytes<float64>(asciiPlyData, out_binaryPlyData);
	}

	PH_ASSERT_UNREACHABLE_SECTION();
	return 0.0;
}

inline void bytes_to_ascii_ply_data(
	const std::byte* const binaryPlyData,
	const EPlyDataType     dataType, 
	std::string* const     out_asciiPlyData)
{
	switch(dataType)
	{
	case EPlyDataType::PPT_int8:
		bytes_to_ascii_ply_data<int8>(binaryPlyData, out_asciiPlyData);
		break;

	case EPlyDataType::PPT_uint8:
		bytes_to_ascii_ply_data<uint8>(binaryPlyData, out_asciiPlyData);
		break;

	case EPlyDataType::PPT_int16:
		bytes_to_ascii_ply_data<int16>(binaryPlyData, out_asciiPlyData);
		break;

	case EPlyDataType::PPT_uint16:
		bytes_to_ascii_ply_data<uint16>(binaryPlyData, out_asciiPlyData);
		break;

	case EPlyDataType::PPT_int32:
		bytes_to_ascii_ply_data<int32>(binaryPlyData, out_asciiPlyData);
		break;

	case EPlyDataType::PPT_uint32:
		bytes_to_ascii_ply_data<uint32>(binaryPlyData, out_asciiPlyData);
		break;

	case EPlyDataType::PPT_float32:
		bytes_to_ascii_ply_data<float32>(binaryPlyData, out_asciiPlyData);
		break;

	case EPlyDataType::PPT_float64:
		bytes_to_ascii_ply_data<float64>(binaryPlyData, out_asciiPlyData);
		break;
	}

	PH_ASSERT_UNREACHABLE_SECTION();
}

}// end anonymous namespace

PlyProperty::PlyProperty() :
	name              (),
	dataType          (EPlyDataType::UNSPECIFIED),
	listSizeType      (EPlyDataType::UNSPECIFIED),
	fixedListSize     (0),
	rawListBuffer     (),
	listSizesPrefixSum()
{}

bool PlyProperty::isList() const
{
	return listSizeType != EPlyDataType::UNSPECIFIED;
}

bool PlyProperty::isFixedSizeList() const
{
	return isList() && fixedListSize > 0;
}

PlyElement::PlyElement() :
	name       (),
	numElements(0),
	properties (),
	rawBuffer  (),
	strideSize (0)
{}

bool PlyElement::isLoaded() const
{
	// First see if there is any data in the raw buffer
	if(!rawBuffer.empty())
	{
		return true;
	}
	// Otherwise, see if there is any data in the list buffer
	else
	{
		for(const PlyProperty& prop : properties)
		{
			if(!prop.rawListBuffer.empty())
			{
				return true;
			}
		}
	}

	return false;
}

PlyPropertyValues::PlyPropertyValues() :
	PlyPropertyValues(nullptr, 0, 0, EPlyDataType::UNSPECIFIED)
{}

PlyPropertyValues::PlyPropertyValues(
	std::byte* const   rawBuffer,
	const std::size_t  strideSize,
	const std::size_t  numElements,
	const EPlyDataType valueType) :

	m_rawBuffer  (rawBuffer),
	m_strideSize (strideSize),
	m_numElements(numElements),
	m_valueType  (valueType)
{}

std::size_t PlyPropertyValues::getBufferOffset(const std::size_t index) const
{
	if(index >= size())
	{
		throw std::out_of_range(std::format(
			"PLY property value index {} exceeded element size {}.",
			index, size()));
	}

	return index * m_strideSize;
}

float64 PlyPropertyValues::get(const std::size_t index) const
{
	PH_ASSERT(m_rawBuffer);
	return bytes_to_ply_data(
		&(m_rawBuffer[getBufferOffset(index)]),
		m_valueType);
}

void PlyPropertyValues::set(const std::size_t index, const float64 value)
{
	PH_ASSERT(m_rawBuffer);
	ply_data_to_bytes(
		value,
		m_valueType,
		&(m_rawBuffer[getBufferOffset(index)]));
}

std::size_t PlyPropertyValues::size() const
{
	return m_numElements;
}

PlyPropertyListValues::PlyPropertyListValues() :
	PlyPropertyListValues(nullptr, nullptr, 0, 0, EPlyDataType::UNSPECIFIED)
{}

PlyPropertyListValues::PlyPropertyListValues(
	std::byte* const   rawBuffer,
	std::size_t* const listSizesPrefixSum,
	const std::size_t  numLists,
	const std::size_t  fixedListSize,
	const EPlyDataType valueType) :

	m_rawBuffer         (rawBuffer),
	m_listSizesPrefixSum(listSizesPrefixSum),
	m_numLists          (numLists),
	m_fixedListSize     (fixedListSize),
	m_valueType         (valueType)
{}

std::size_t PlyPropertyListValues::getBufferOffset(const std::size_t listIndex, const std::size_t listElementIndex) const
{
	const auto numLists      = size();
	const auto numListValues = listSize(listIndex);

	if(listIndex >= numLists || listElementIndex >= numListValues)
	{
		throw std::out_of_range(std::format(
			"PLY list index {} and list element index {} overflow: # lists = {}, # list values = {}.",
			listIndex, listElementIndex, numLists, numListValues));
	}

	PH_ASSERT(
		isFixedSizeList() ||
		(!isFixedSizeList() && m_listSizesPrefixSum));

	const auto sizeofValue = sizeof_ply_data_type(m_valueType);

	return isFixedSizeList()
		? (listIndex * m_fixedListSize + listElementIndex) * sizeofValue
		: (m_listSizesPrefixSum[listIndex] + listElementIndex) * sizeofValue;
}

float64 PlyPropertyListValues::get(const std::size_t listIndex, const std::size_t listElementIndex) const
{
	PH_ASSERT(m_rawBuffer);
	return bytes_to_ply_data(
		&(m_rawBuffer[getBufferOffset(listIndex, listElementIndex)]),
		m_valueType);
}

void PlyPropertyListValues::set(std::size_t listIndex, std::size_t listElementIndex, float64 value)
{
	PH_ASSERT(m_rawBuffer);
	ply_data_to_bytes(
		value,
		m_valueType,
		&(m_rawBuffer[getBufferOffset(listIndex, listElementIndex)]));
}

std::size_t PlyPropertyListValues::size() const
{
	return m_numLists;
}

std::size_t PlyPropertyListValues::listSize(const std::size_t listIndex) const
{
	PH_ASSERT(
		isFixedSizeList() ||
		(!isFixedSizeList() && m_listSizesPrefixSum));

	return isFixedSizeList()
		? m_fixedListSize 
		: m_listSizesPrefixSum[listIndex + 1] - m_listSizesPrefixSum[listIndex];
}

bool PlyPropertyListValues::isFixedSizeList() const
{
	return m_fixedListSize > 0;
}

std::size_t PlyPropertyListValues::fixedListSize() const
{
	return m_fixedListSize;
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

	m_elements.shrink_to_fit();
	for(auto& element : m_elements)
	{
		element.properties.shrink_to_fit();
		for(auto& prop : element.properties)
		{
			prop.rawListBuffer.shrink_to_fit();
			prop.listSizesPrefixSum.shrink_to_fit();
		}

		element.rawBuffer.shrink_to_fit();
	}
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

		// Reserve memory space for the non-list element buffer
		element.rawBuffer.reserve(element.numElements * element.strideSize);

		// Reserve memory space for the list buffer
		for(PlyProperty& prop : element.properties)
		{
			if(prop.isList())
			{
				const auto listSize = prop.isFixedSizeList()
					? prop.fixedListSize
					: 3;// assumed 3 as in most files lists are for triangle vertices

				prop.rawListBuffer.reserve(element.numElements * listSize * sizeof_ply_data_type(prop.dataType));
				prop.listSizesPrefixSum.reserve(element.numElements + 1);
			}
		}
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
				PH_ASSERT(!m_elements.empty());
				PlyElement& currentElement = m_elements.back();

				PlyProperty newProp;

				const auto tokenAfterEntry = next_token(headerLine, &headerLine);
				if(tokenAfterEntry == "list")
				{
					newProp.listSizeType = ply_keyword_to_data_type(next_token(headerLine, &headerLine));
					newProp.dataType     = ply_keyword_to_data_type(next_token(headerLine, &headerLine));
					newProp.name         = next_token(headerLine);
				}
				else
				{
					newProp.dataType = ply_keyword_to_data_type(tokenAfterEntry);
					newProp.name     = next_token(headerLine);

					currentElement.strideSize += sizeof_ply_data_type(newProp.dataType);
				}

				currentElement.properties.push_back(newProp);
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

		// Prepend prefix sums with a 0
		for(PlyProperty& prop : element.properties)
		{
			if(prop.isList())
			{
				PH_ASSERT(prop.listSizesPrefixSum.empty());
				prop.listSizesPrefixSum.push_back(0);
			}
		}

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
						std::array<std::byte, 8> dummyBuffer;
						const auto listSize = static_cast<std::size_t>(ascii_ply_data_to_bytes(
							next_token(currentLine, &currentLine),
							prop.dataType,
							dummyBuffer.data()));

						std::vector<std::byte>& rawBuffer = prop.rawListBuffer;

						const std::size_t firstByteIdx = rawBuffer.size();
						const std::size_t sizeofData   = sizeof_ply_data_type(prop.dataType);
						rawBuffer.resize(rawBuffer.size() + sizeofData * listSize);
						for(std::size_t li = 0; li < listSize; ++li)
						{
							ascii_ply_data_to_bytes(
								next_token(currentLine, &currentLine), 
								prop.dataType, 
								&(rawBuffer[firstByteIdx + li * sizeofData]));
						}

						PH_ASSERT(!prop.listSizesPrefixSum.empty());
						prop.listSizesPrefixSum.push_back(prop.listSizesPrefixSum.back() + listSize);

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
						std::vector<std::byte>& rawBuffer = element.rawBuffer;

						const std::size_t firstByteIdx = rawBuffer.size();
						rawBuffer.resize(rawBuffer.size() + sizeof_ply_data_type(prop.dataType));

						ascii_ply_data_to_bytes(
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

		// Remove prefix sums if the list has fixed size
		for(PlyProperty& prop : element.properties)
		{
			if(prop.isFixedSizeList())
			{
				std::vector<std::size_t>().swap(prop.listSizesPrefixSum);
			}
		}
	}
}

void PlyFile::loadBinaryBuffer(IInputStream& stream, const PlyIOConfig& config, const Path& plyFilePath)
{
	// TODO
	PH_ASSERT_UNREACHABLE_SECTION();
}

}// end namespace ph
