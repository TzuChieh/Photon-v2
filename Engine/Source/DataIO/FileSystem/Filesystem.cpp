#include "DataIO/FileSystem/Filesystem.h"
#include "DataIO/FileSystem/Path.h"
#include "DataIO/io_exceptions.h"

#include <filesystem>
#include <format>

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

void Filesystem::copyFile(
	const Path& srcFile,
	const Path& dstFile,
	bool overwriteExisting)
{
	auto options = std::filesystem::copy_options::skip_existing;
	if(overwriteExisting)
	{
		options = std::filesystem::copy_options::overwrite_existing;
	}

	std::error_code errorCode;
	std::filesystem::copy_file(srcFile.toStdPath(), dstFile.toStdPath(), options, errorCode);
	if(errorCode)
	{
		throw FilesystemError(std::format(
			"Error copying file from \"{}\" to \"{}\".", srcFile, dstFile),
			errorCode);
	}
}

void Filesystem::copyDirectories(
	const Path& srcDir,
	const Path& dstDir,
	bool overwriteExisting)
{
	auto options = std::filesystem::copy_options::skip_existing;
	if(overwriteExisting)
	{
		options = std::filesystem::copy_options::overwrite_existing;
	}

	// This method always copy all subdirectories and their content recursively
	options |= std::filesystem::copy_options::recursive;

	std::error_code errorCode;
	std::filesystem::copy(srcDir.toStdPath(), dstDir.toStdPath(), options, errorCode);
	if(errorCode)
	{
		throw FilesystemError(std::format(
			"Error copying directories from \"{}\" to \"{}\".", srcDir, dstDir),
			errorCode);
	}
}

Path Filesystem::makeRelative(const Path& src, const Path& base)
{
	return Path(std::filesystem::relative(src.toStdPath(), base.toStdPath()));
}

}// end namespace ph
