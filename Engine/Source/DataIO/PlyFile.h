#pragma once

/*! @file

@brief PLY (Polygon File Format) IO operations.

References:
[1] PLY format specification: http://paulbourke.net/dataformats/ply/
*/

#include "DataIO/FileSystem/Path.h"

#include <vector>
#include <cstddef>
#include <string>

namespace ph { class BinaryFileInputStream; }

namespace ph
{

enum class EPlyFileFormat
{
	ASCII,
	Binary,
	BinaryBigEndian
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
	PPT_float64
};

struct PlyIOConfig final
{
	bool isCommentsIgnored = true;
};

class PlyFile final
{
public:
	PlyFile();
	explicit PlyFile(const Path& plyFilePath);
	PlyFile(const Path& plyFilePath, const PlyIOConfig& config);

	void setFormat(EPlyFileFormat format);
	void clear();

private:
	struct PlyProperty final
	{
		std::string  name;
		EPlyDataType dataType;
		EPlyDataType listCountType;

		bool isList() const;
	};

	struct PlyElement final
	{
		std::string              name;
		std::size_t              numElements;
		std::vector<PlyProperty> properties;
		std::vector<std::byte>   rawBuffer;
	};

	void readHeader(BinaryFileInputStream& stream);

private:
	EPlyFileFormat           m_format = EPlyFileFormat::ASCII;
	std::vector<std::string> m_comments;
	std::vector<PlyElement>  m_elements;
};

}// end namespace ph
