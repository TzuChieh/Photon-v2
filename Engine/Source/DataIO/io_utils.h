#pragma once

/*! @file

@brief Miscellaneous file input & output utilities.
*/

#include "DataIO/FileSystem/Path.h"
#include "Common/primitive_type.h"

#include <string>
#include <string_view>

namespace ph { class RegularPicture; }

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

/*! @brief Loads a picture as a HDR frame.
*/
RegularPicture load_picture(const Path& picturePath);
RegularPicture load_LDR_picture(const Path& picturePath);
RegularPicture load_HDR_picture(const Path& picturePath);

}// end namespace ph::io_utils
