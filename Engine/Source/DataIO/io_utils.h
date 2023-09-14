#pragma once

/*! @file

@brief Miscellaneous file input & output utilities.
*/

#include "Common/primitive_type.h"
#include "Frame/frame_fwd.h"
#include "Math/math_fwd.h"

#include <string_view>
#include <string>

namespace ph { class Path; }
namespace ph { class RegularPicture; }
namespace ph { class RegularPictureFormat; }

namespace ph::io_utils
{

/*! @brief Read the whole file as a formatted string.
*/
std::string load_text(const Path& filePath);

// TODO: loadBytes()

/*! @brief Checks whether the provided picture format supports LDR by filename extension.

Note that the check is from the engine's perspective--a format may not
support LDR here while it actually does from its original spec.

@param filenameExt Filename extension (the leading dot should be included).
*/
bool has_LDR_support(std::string_view filenameExt);

/*! @brief Checks whether the provided picture format supports HDR by filename extension.

Note that the check is from the engine's perspective--a format may not
support HDR here while it actually does from its original spec.

@param filenameExt Filename extension (the leading dot should be included).
*/
bool has_HDR_support(std::string_view filenameExt);

/*! @brief Loads common picture types from file.
Format is deduced from filename extension.
@exception FileIOError If any error occurred.
*/
///@{
RegularPicture load_picture(const Path& picturePath);
RegularPicture load_LDR_picture(const Path& picturePath);
RegularPicture load_HDR_picture(const Path& picturePath);
///@}

bool load_picture_meta(
	const Path& picturePath, 
	math::Vector2S* out_sizePx,
	std::size_t* out_numComponents = nullptr,
	RegularPictureFormat* out_format = nullptr);

void save(const LdrRgbFrame& frame, const Path& picturePath);

/*! @brief Saves a HDR frame to the specified file.
Notice that if the specified format is LDR, values outside [0, 1] will be clamped. Actual format 
is deduced from filename extension.
*/
void save(const HdrRgbFrame& frame, const Path& picturePath);

void save_png(const LdrRgbFrame& frame, const Path& picturePath);
void save_jpg(const LdrRgbFrame& frame, const Path& picturePath);
void save_bmp(const LdrRgbFrame& frame, const Path& picturePath);
void save_tga(const LdrRgbFrame& frame, const Path& picturePath);
void save_hdr(const HdrRgbFrame& frame, const Path& picturePath);
void save_exr(const HdrRgbFrame& frame, const Path& picturePath);
void save_exr_high_precision(const HdrRgbFrame& frame, const Path& picturePath);
void save_pfm(const HdrRgbFrame& frame, const Path& picturePath);

// HACK
void save_exr(const HdrRgbFrame& frame, std::string& byteBuffer);

namespace detail
{

bool init_picture_IO();

}// end namespace detail

}// end namespace ph::io_utils
