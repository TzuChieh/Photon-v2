#pragma once

#include <ph_c_core.h>

#include <string>

namespace ph::cli
{

void save_frame_with_fail_safe(PHuint64 frameId, const std::string& filePath);

}// end namespace ph::cli
