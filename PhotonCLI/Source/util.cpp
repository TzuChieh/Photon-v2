#include "util.h"

#include <ph_c_core.h>
#include <Common/logging.h>

namespace ph::cli
{

void save_frame_with_fail_safe(
	const PhUInt64 frameId, 
	const std::string& filePath,
	const PhFrameSaveInfo* const saveInfo)
{
	PH_DEFAULT_LOG(Note, "saving image to {}", filePath);

	if(!phSaveFrame(frameId, filePath.c_str(), saveInfo))
	{
		PH_DEFAULT_LOG(Warning,
			"Image saving failed. In case precious data will be lost, "
			"the image will be saved in higher precision (.exr format).");

		if(!phSaveFrame(frameId, (filePath + "_failsafe.exr").c_str(), saveInfo))
		{
			PH_DEFAULT_LOG(Warning,
				"Image saving failed. Will try saving again in .pfm (without extra info).");

			if(!!phSaveFrame(frameId, (filePath + "_failsafe.pfm").c_str(), nullptr))
			{
				PH_DEFAULT_LOG(Error, "Image saving failed again.");
			}
		}
	}
}

}// end namespace ph::cli
