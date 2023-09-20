#pragma once

/*! @file

@brief PLY (Polygon File Format) IO operations.

References:
[1] PLY format specification: http://paulbourke.net/dataformats/ply/
*/

#include "DataIO/FileSystem/Path.h"
#include "Utility/SemanticVersion.h"
#include "Math/constant.h"
#include "Common/primitive_type.h"

#include <vector>
#include <cstddef>
#include <string>
#include <string_view>
#include <climits>

namespace ph { class IInputStream; }

namespace ph
{

enum class EPlyDataFormat
{
	ASCII = 0,
	BinaryLittleEndian,
	BinaryBigEndian,

	NUM
};

enum class EPlyDataType
{
	Unspecified = 0,

	/*! The `char` type, character with 1 byte. */
	Int8,

	/*! The `uchar` type, unsigned character with 1 byte. */
	UInt8,

	/*! The `short` type, short integer with 2 bytes. */
	Int16,

	/*! The `ushort` type, unsigned short integer with 2 bytes. */
	UInt16,

	/*! The `int` type, integer with 4 bytes. */
	Int32,

	/*! The `uint` type, integer with 4 bytes. */
	UInt32,

	/*! The `float` type, single-precision float with 4 bytes. */
	Float32,

	/*! The `double` type, double-precision float with 8 bytes. */
	Float64,

	NUM
};

struct PlyIOConfig final
{
	/*! Whether to load/save comments in the file. */
	bool bIgnoreComments = true;

	/*! Preload the file into memory to increase parsing performance. */
	bool bPreloadIntoMemory = true;

	/*! Only preload files with size smaller than the threshold. */
	std::size_t preloadMemoryThreshold = 1 * math::constant::GiB;

	/*! Reduce memory used for storage by the PlyFile class for files with size larger than the threshold. */
	std::size_t reduceStorageMemoryThreshold = 768 * math::constant::MiB;
};

class PlyPropertyValues;
class PlyPropertyListValues;

/*! @brief PLY property storage.
*/
struct PlyProperty final
{
	std::string  name;
	EPlyDataType dataType;
	EPlyDataType listSizeType;
	std::size_t  strideOffset;
	std::size_t  fixedListSize;

	/*! Raw data storage for a single list property only. Do not contain size information. 
	See `PlyElement::rawBuffer` for why lists have their own data storages.
	*/
	std::vector<std::byte> rawListBuffer;

	/*! For variable-sized list to calculate byte offset and list size in O(1). Not needed for 
	fixed size list.
	*/
	std::vector<std::size_t> listSizesPrefixSum;

	PlyProperty();

	bool isList() const;
	bool isFixedSizeList() const;
};

/*! @brief PLY element storage.
*/
struct PlyElement final
{
	std::string              name;
	std::size_t              numElements;
	std::vector<PlyProperty> properties;
	std::size_t              strideSize;

	/*! Raw data storage for properties in a PLY element. Note that this buffer is for non-list
	properties only, as list may be variable-sized which will make properties no longer locatable
	via a constant stride size (properties in a PLY element is stored in AoS). Fixed size lists,
	in theory, can fit into non-list properties without problem; however, most data have list
	properties separated into its own element already and we find it is also easier to deal with
	lists with the current policy.
	Each list has its own raw data storage in `PlyProperty::rawListBuffer`.
	*/
	std::vector<std::byte> rawBuffer;

	PlyElement();

	bool isLoaded() const;
	bool containsList() const;
	PlyProperty* findProperty(std::string_view name);
	PlyPropertyValues propertyValues(PlyProperty* prop);
	PlyPropertyListValues listPropertyValues(PlyProperty* prop);
};

/*! @brief A convenient PLY property accessor.
*/
class PlyPropertyValues final
{
	friend PlyElement;

public:
	float64 get(std::size_t index) const;
	void set(std::size_t index, float64 value);
	std::size_t size() const;
	operator bool () const;

private:
	PlyPropertyValues();

	PlyPropertyValues(
		std::byte*   rawBuffer,
		std::size_t  strideSize,
		std::size_t  numElements,
		EPlyDataType valueType);

	std::size_t getBufferOffset(std::size_t index) const;

	std::byte*   m_rawBuffer;
	std::size_t  m_strideSize;
	std::size_t  m_numElements;
	EPlyDataType m_valueType;
};

/*! @brief A convenient PLY list property accessor.
*/
class PlyPropertyListValues final
{
	friend PlyElement;

public:
	float64 get(std::size_t listIndex, std::size_t listElementIndex) const;
	void set(std::size_t listIndex, std::size_t listElementIndex, float64 value);
	
	/*! @brief Number of lists in this property.
	*/
	std::size_t size() const;

	/*! @brief Size of the list on index @p listIndex.
	*/
	std::size_t listSize(std::size_t listIndex) const;

	bool isFixedSizeList() const;
	std::size_t fixedListSize() const;
	operator bool () const;

private:
	PlyPropertyListValues();

	PlyPropertyListValues(
		std::byte*   rawBuffer,
		std::size_t* listSizesPrefixSum,
		std::size_t  numLists,
		std::size_t  fixedListSize,
		EPlyDataType valueType);

	std::size_t getBufferOffset(std::size_t listIndex, std::size_t listElementIndex) const;

	std::byte*   m_rawBuffer;
	std::size_t* m_listSizesPrefixSum;
	std::size_t  m_numLists;
	std::size_t  m_fixedListSize;
	EPlyDataType m_valueType;
};

class PlyFile final
{
	static_assert(sizeof(std::byte) * CHAR_BIT == 8,
		"The file explicitly depends on the fact that std::byte contains 8 bits.");

public:
	PlyFile();

	/*! @brief Load file with default config.
	*/
	explicit PlyFile(const Path& plyFilePath);

	/*! @brief Load file with specified config.
	@exception FileIOError If any loading and parsing process failed.
	*/
	PlyFile(const Path& plyFilePath, const PlyIOConfig& config);

	PlyElement* findElement(std::string_view name);
	std::size_t numElements() const;

	EPlyDataFormat getInputFormat() const;
	void setInputFormat(EPlyDataFormat format);
	EPlyDataFormat getOutputFormat() const;
	void setOutputFormat(EPlyDataFormat format);

	/*! @brief Access to comments in the file.
	There will be no comments if PlyIOConfig::bIgnoreComments is set.
	*/
	///@{
	std::size_t numComments() const;
	std::string_view getComment(std::size_t commentIndex) const;
	///@}

	/*! @brief Load and append file content to existing data.
	*/
	void loadFile(const Path& plyFilePath, const PlyIOConfig& config);

	/*! @brief Clear all data storages for the elements.
	*/
	void clearBuffer();

	SemanticVersion getVersion() const;

private:
	/*! Parse and append to existing header info. */
	void parseHeader(IInputStream& stream, const PlyIOConfig& config);

	/*! Load data described by header into memory. Already loaded elements do not participate in
	the data description of the current buffer/stream.
	*/
	void loadBuffer(IInputStream& stream, const PlyIOConfig& config);

	/*! Load PLY element data in ASCII form into memory. */
	void loadAsciiElementBuffer(
		IInputStream& stream, 
		PlyElement& element, 
		const PlyIOConfig& config);

	/*! Load PLY element data in binary form into memory. */
	void loadBinaryElementBuffer(
		IInputStream& stream, 
		PlyElement& element, 
		const PlyIOConfig& config);

	/*! Load non-list PLY element data in binary form into memory. */
	void loadNonListBinaryElementBuffer(
		IInputStream& stream,
		PlyElement& element,
		const PlyIOConfig& config);

	void loadSingleBinaryPlyDataToBuffer(
		IInputStream& stream, 
		EPlyDataType dataType, 
		std::byte* out_buffer);

	/*! Try to make internal memory footprint smaller after the data is loaded. */
	void compactBuffer();

	/*! Reserve memory space before loading so there can be fewer dynamic allocations during load. */
	void reserveBuffer();

private:
	EPlyDataFormat           m_inputFormat;
	EPlyDataFormat           m_outputFormat;
	EPlyDataFormat           m_nativeFormat;
	SemanticVersion          m_version;
	std::vector<std::string> m_comments;
	std::vector<PlyElement>  m_elements;
};

}// end namespace ph
