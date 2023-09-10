#include "DataIO/FileSystem/Filesystem.h"
#include "DataIO/FileSystem/Path.h"

#include <filesystem>

namespace ph
{

bool Filesystem::hasDirectory(const Path& path)
{
	return std::filesystem::is_directory(path.toStdPath());
}
	
bool Filesystem::hasFile(const Path& path)
{
	return std::filesystem::is_regular_file(path.toStdPath());
}

void Filesystem::createDirectory(const Path& path)
{
	// TODO: throw on error

	std::filesystem::create_directory(path.toStdPath());
}

void Filesystem::createDirectories(const Path& path)
{
	// TODO: throw on error

	std::filesystem::create_directories(path.toStdPath());
}

Path Filesystem::makeRelative(const Path& src, const Path& base)
{
	return Path(std::filesystem::relative(src.toStdPath(), base.toStdPath()));
}

}// end namespace ph
