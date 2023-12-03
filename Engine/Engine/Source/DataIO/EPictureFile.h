#pragma once

#include <string_view>

namespace ph
{

enum class EPictureFile
{
	Unknown = 0,
	PNG,
	JPG,
	BMP,
	TGA,
	HDR,
	EXR,
	HighPrecisionEXR,
	PFM
};

EPictureFile picture_file_type_from_extension(std::string_view ext);

}// end namespace ph
