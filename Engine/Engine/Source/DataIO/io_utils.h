#pragma once

/*! @file

@brief Miscellaneous file input & output utilities.
*/

#include "Frame/frame_fwd.h"
#include "Math/math_fwd.h"
#include "DataIO/EPictureFile.h"

#include <Common/primitive_type.h>

#include <cstddef>
#include <string_view>
#include <string>

namespace ph { class Path; }
namespace ph { class RegularPicture; }
namespace ph { class RegularPictureFormat; }
namespace ph { class PictureMeta; }
namespace ph { class ByteBuffer; }

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
Format is deduced from filename extension. Tries to load the specified layer into a regular picture.
@param layerIdx Index of the layer to load. For most picture type, left the layer index as default
should be good.
@exception FileIOError If any error occurred.
*/
///@{
RegularPicture load_picture(const Path& picturePath, std::size_t layerIdx = 0);
RegularPicture load_LDR_picture(const Path& picturePath, std::size_t layerIdx = 0);
RegularPicture load_HDR_picture(const Path& picturePath, std::size_t layerIdx = 0);
///@}

/*! @brief Load general information that describes the picture.
For most formats, this parses the image header without loading actual pixel data.
@param out_meta Stores the loaded picture meta. Can be set to `nullptr` if only the selected information
is required (use the more specific output variable instead).
*/
bool load_picture_meta(
	const Path& picturePath, 
	PictureMeta* out_meta,
	math::Vector2S* out_sizePx = nullptr,
	std::size_t* out_numChannels = nullptr);

/*! @brief Saves a LDR frame to the specified file.
Similar to `save(4)`, except file name and format are deduced from path.
*/
void save(const LdrRgbFrame& frame, const Path& filePath, const PictureMeta* meta = nullptr);

/*! @brief Saves a HDR frame to the specified file.
Similar to `save(4)`, except file name and format are deduced from path.
*/
void save(const HdrRgbFrame& frame, const Path& filePath, const PictureMeta* meta = nullptr);

/*! @brief Saves a LDR frame to the specified file.
Notice that if the specified format is HDR, values will be promoted to higher precision types which
can have extra storage cost.
@param meta General information for storing the frame as a picture. The information given may or may
not be considered (depending on the file format).
*/
void save(
	const LdrRgbFrame& frame, 
	const Path& fileDirectory, 
	const std::string& name, 
	EPictureFile format,
	const PictureMeta* meta = nullptr);

/*! @brief Saves a HDR frame to the specified file.
Notice that if the specified format is LDR, values outside [0, 1] will be clamped (this may result
in significant data loss depending on the values stored).
@param meta General information for storing the frame as a picture. The information given may or may
not be considered (depending on the file format).
*/
void save(
	const HdrRgbFrame& frame, 
	const Path& fileDirectory, 
	const std::string& name, 
	EPictureFile format,
	const PictureMeta* meta = nullptr);

/*! @brief Saves a frame in PNG format.
*/
void save_png(const LdrRgbFrame& frame, const Path& filePath, const PictureMeta* meta = nullptr);

/*! @brief Saves a frame in JPG format.
*/
void save_jpg(const LdrRgbFrame& frame, const Path& filePath, const PictureMeta* meta = nullptr);

/*! @brief Saves a frame in BMP format.
*/
void save_bmp(const LdrRgbFrame& frame, const Path& filePath, const PictureMeta* meta = nullptr);

/*! @brief Saves a frame in TGA format.
*/
void save_tga(const LdrRgbFrame& frame, const Path& filePath, const PictureMeta* meta = nullptr);

/*! @brief Saves a frame in HDR format.
*/
void save_hdr(const HdrRgbFrame& frame, const Path& filePath, const PictureMeta* meta = nullptr);

/*! @brief Saves a frame in EXR format.
*/
void save_exr(const HdrRgbFrame& frame, const Path& filePath, const PictureMeta* meta = nullptr);

/*! @brief Saves a frame in EXR format with high precision encoding.
*/
void save_exr_high_precision(const HdrRgbFrame& frame, const Path& filePath, const PictureMeta* meta = nullptr);

/*! @brief Saves a frame in PFM format.
*/
void save_pfm(const HdrRgbFrame& frame, const Path& filePath, const PictureMeta* meta = nullptr);

/*! @brief Saves a frame in EXR format to memory.
*/
void save_exr(const HdrRgbFrame& frame, ByteBuffer& buffer, const PictureMeta* meta = nullptr);

namespace detail
{

bool init_picture_IO();

}// end namespace detail

}// end namespace ph::io_utils
