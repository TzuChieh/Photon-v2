#pragma once

#include "DataIO/FileSystem/Path.h"

#include <string>

namespace ph::io_utils
{

/*! @brief Read the whole file as string, unformatted.
*/
std::string load_text(const Path& filePath);

// TODO: loadBytes()

}// end namespace ph::io_utils
