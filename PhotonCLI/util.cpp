#include "util.h"

#include <iostream>

namespace ph::cli
{

void save_frame_with_fail_safe(const PHuint64 frameId, const std::string& filePath)
{
	std::cout << "saving image to <" << filePath << ">" << std::endl;

	if(phSaveFrame(frameId, filePath.c_str()) == PH_FALSE)
	{
		std::cerr << "Image saving failed. In case precious data will be lost, "
		          << "the image is going to be saved in high precision exr format." << std::endl;

		if(phSaveFrame(frameId, (filePath + "_failsafe.exr").c_str()) == PH_FALSE)
		{
			std::cerr << "Image saving failed again." << std::endl;
		}
	}
}

}// end namespace ph::cli
