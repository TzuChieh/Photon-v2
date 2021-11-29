#pragma once

/*! @file

@brief PLY (Polygon File Format) IO operations.

References:
[1] PLY format specification: http://paulbourke.net/dataformats/ply/
*/

#include "DataIO/FileSystem/Path.h"
#include "Utility/SemanticVersion.h"

#include <vector>
#include <cstddef>
#include <string>
#include <string_view>

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
	bool bIgnoreComments    = true;
	bool bPreloadIntoMemory = true;
};

class PlyFile final
{
public:
	PlyFile();
	explicit PlyFile(const Path& plyFilePath);
	PlyFile(const Path& plyFilePath, const PlyIOConfig& config);

	void setFormat(EPlyFileFormat format);
	void clear();
	SemanticVersion getVersion() const;

private:
	struct PlyProperty final
	{
		std::string  name;
		EPlyDataType dataType;
		EPlyDataType listCountType;

		PlyProperty();

		bool isList() const;
	};

	struct PlyElement final
	{
		std::string              name;
		std::size_t              numElements;
		std::vector<PlyProperty> properties;
		std::vector<std::byte>   rawBuffer;

		PlyElement();
	};

	void readHeader(std::string_view headerStr, const PlyIOConfig& config, const Path& plyFilePath);

private:
	EPlyFileFormat           m_format;
	SemanticVersion          m_version;
	std::vector<std::string> m_comments;
	std::vector<PlyElement>  m_elements;
};

}// end namespace ph
