#include "Common/config.h"
#include "Common/assertion.h"

namespace ph
{

std::string& Config::RENDERER_RESOURCE_DIRECTORY()
{
	static std::string directory("./Photon-v2-Resource/Resource/");

	// TODO: assert is folder
	return directory;
}

}// end namespace ph