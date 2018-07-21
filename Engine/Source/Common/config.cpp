#include "Common/config.h"
#include "FileIO/FileSystem/Path.h"
#include "Common/assertion.h"

namespace ph
{

Path& Config::CORE_RESOURCE_DIRECTORY()
{
	static Path directory("./Resource/");

	// TODO: assert is folder
	return directory;
}

}// end namespace ph