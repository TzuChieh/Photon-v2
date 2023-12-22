#pragma once

#include <ph_c_core_types.h>

#include <string>

namespace ph::cli
{

void save_frame_with_fail_safe(
	PhUInt64 frameId, 
	const std::string& filePath,
	const PhFrameSaveInfo* saveInfo = nullptr);

}// end namespace ph::cli
