#pragma once

/*! @file

@brief PLY (Polygon File Format) IO operations.

References:
[1] PLY format specification: http://paulbourke.net/dataformats/ply/
*/

namespace ph
{

enum class EPlyFileFormat
{
	ASCII,
	Binary,
	BinaryBigEndian
};

enum class EPlyPropertyType
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

class PlyFile final
{
public:

private:

};

}// end namespace ph
