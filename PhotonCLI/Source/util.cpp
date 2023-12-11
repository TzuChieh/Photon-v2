#include "util.h"

#include <Common/logging.h>

namespace ph::cli
{

void save_frame_with_fail_safe(const PhUInt64 frameId, const std::string& filePath)
{
	PH_DEFAULT_LOG("saving image to {}", filePath);

	if(!phSaveFrame(frameId, filePath.c_str(), nullptr))
	{
		PH_DEFAULT_LOG_WARNING(
			"Image saving failed. In case precious data will be lost, "
			"the image is going to be saved in high precision exr format.");

		if(!phSaveFrame(frameId, (filePath + "_failsafe.exr").c_str(), nullptr))
		{
			PH_DEFAULT_LOG_ERROR("Image saving failed again.");
		}
	}
}

}// end namespace ph::cli
