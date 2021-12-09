#pragma once

/*! @file

@brief PLY (Polygon File Format) IO operations.

References:
[1] PLY format specification: http://paulbourke.net/dataformats/ply/
*/

#include "DataIO/FileSystem/Path.h"
#include "Utility/SemanticVersion.h"
#include "Math/constant.h"

#include <vector>
#include <cstddef>
#include <string>
#include <string_view>
#include <climits>

namespace ph { class IInputStream; }

namespace ph
{

enum class EPlyFileFormat
{
	ASCII = 0,
	Binary,
	BinaryBigEndian,

	NUM
};

enum class EPlyDataType
{
	UNSPECIFIED = 0,

	/*! The `char` type, character with 1 byte. */
	PPT_int8,

	/*! The `uchar` type, unsigned character with 1 byte. */
	PPT_uint8,

	/*! The `short` type, short integer with 2 bytes. */
	PPT_int16,

	/*! The `ushort` type, unsigned short integer with 2 bytes. */
	PPT_uint16,

	/*! The `int` type, integer with 4 bytes. */
	PPT_int32,

	/*! The `uint` type, integer with 4 bytes. */
	PPT_uint32,

	/*! The `float` type, single-precision float with 4 bytes. */
	PPT_float32,

	/*! The `double` type, double-precision float with 8 bytes. */
	PPT_float64,

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

struct PlyProperty final
{
	std::string              name;
	EPlyDataType             dataType;
	EPlyDataType             listSizeType;
	std::size_t              fixedListSize;
	std::vector<std::byte>   rawListBuffer;
	std::vector<std::size_t> listSizesPrefixSum;

	PlyProperty();

	bool isList() const;
	bool isFixedSizeList() const;
};

struct PlyElement final
{
	std::string              name;
	std::size_t              numElements;
	std::vector<PlyProperty> properties;
	std::vector<std::byte>   rawBuffer;
	std::size_t              strideSize;

	PlyElement();

	bool isLoaded() const;
};

class PlyFile final
{
	static_assert(sizeof(std::byte)* CHAR_BIT == 8,
		"The file explicitly depends on the fact that std::byte contains 8 bits.");

public:
	PlyFile();
	explicit PlyFile(const Path& plyFilePath);
	PlyFile(const Path& plyFilePath, const PlyIOConfig& config);

	const PlyElement* findElement(std::string_view name) const;
	std::size_t numElements() const;
	EPlyFileFormat getFormat() const;
	void setFormat(EPlyFileFormat format);

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
	void parseHeader(IInputStream& stream, const PlyIOConfig& config, const Path& plyFilePath);
	void loadTextBuffer(IInputStream& stream, const PlyIOConfig& config, const Path& plyFilePath);
	void loadBinaryBuffer(IInputStream& stream, const PlyIOConfig& config, const Path& plyFilePath);
	void compactBuffer();
	void reserveBuffer();

private:
	EPlyFileFormat           m_format;
	SemanticVersion          m_version;
	std::vector<std::string> m_comments;
	std::vector<PlyElement>  m_elements;
};

}// end namespace ph
