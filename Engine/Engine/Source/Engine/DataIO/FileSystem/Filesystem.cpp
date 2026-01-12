#include "Engine/DataIO/FileSystem/Filesystem.h"
#include "Engine/DataIO/FileSystem/Path.h"

#include <Common/io_exceptions.h>
#include <Common/os.h>

#include <filesystem>
#include <array>

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

void Filesystem::copyFileToDirectory(
	const Path& srcFile,
	const Path& dstDir,
	bool overwriteExisting,
	bool createMissingDirectories)
{
	if(createMissingDirectories && !hasDirectory(dstDir))
	{
		createDirectories(dstDir);
	}

	copyFile(srcFile, dstDir / srcFile.getFilename(), overwriteExisting);
}

void Filesystem::copy(
	const Path& srcPath,
	const Path& dstPath,
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
	std::filesystem::copy(srcPath.toStdPath(), dstPath.toStdPath(), options, errorCode);
	if(errorCode)
	{
		throw FilesystemError(std::format(
			"Error copying directories from \"{}\" to \"{}\".", srcPath, dstPath),
			errorCode);
	}
}

Path Filesystem::makeRelative(const Path& src, const Path& base)
{
	return Path(std::filesystem::relative(src.toStdPath(), base.toStdPath()));
}

const Path& Filesystem::getExecutablePath()
{
	// Cache this as it will not change during runtime
	static auto path = Path(os::get_executable_path());
	return path;
}

const Path& Filesystem::getInstallationDirectory()
{
	// Installation path is the directory outside of "bin"
	static auto path = getExecutablePath().getParent().getParent().toCanonical();
	return path;
}

const Path& Filesystem::getConfigDirectory()
{
	static auto path = getInstallationDirectory() / "Config";
	return path;
}

const Path& Filesystem::getScriptDirectory()
{
	static auto path = getInstallationDirectory() / "Script";
	return path;
}

const Path& Filesystem::getIntermediateDirectory()
{
	static auto path = getInstallationDirectory() / "Intermediate";
	return path;
}

const Path& Filesystem::getInternalResourceDirectory()
{
	static auto path = getInstallationDirectory() / "InternalResource";
	return path;
}

const Path& Filesystem::getResourceDirectory()
{
	static auto path = getInstallationDirectory() / "Resource";
	return path;
}

}// end namespace ph
