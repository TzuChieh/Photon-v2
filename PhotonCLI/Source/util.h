#pragma once

#include <CEngine/ph_c_core_types.h>

#include <string>
#include <string_view>

namespace ph::cli
{

void save_frame_with_fail_safe(
	PhUInt64 frameId, 
	const std::string& filePath,
	const PhFrameSaveInfo* saveInfo = nullptr,
	std::string_view layerName = "");

}// end namespace ph::cli
