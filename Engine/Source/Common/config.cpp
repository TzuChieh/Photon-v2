#include "Common/config.h"
#include "Common/assertion.h"

namespace ph
{

std::string& Config::CORE_RESOURCE_DIRECTORY()
{
	static std::string directory("./Resource/");

	// TODO: assert is folder
	return directory;
}

}// end namespace ph