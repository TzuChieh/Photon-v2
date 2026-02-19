#include "Engine/DataIO/FileSystem/Filesystem.h"

#include <Common/io_exceptions.h>
#include <Common/os.h>

#include <filesystem>
#include <array>

namespace ph
{

std::optional<Path> Filesystem::s_installationDirectory;

Path Filesystem::findInstallationDirectory(const std::string& referenceEngineDir)
{
	// There is a "PhotonRenderer.info" file at the installation root. We will climb back up the
	// directory tree until we find it, or we reach the root directory. This may be called before
	// engine init, so we rely only on STL as much as possible.

	std::filesystem::path current = referenceEngineDir.empty()
		? os::get_executable_path().parent_path()
		: std::filesystem::path(referenceEngineDir);
	current = std::filesystem::canonical(current);

	// Limit the number of iterations to avoid infinite loop in case of unexpected directory structure.
	for(int i = 0; i < 20; ++i)
	{
		if(std::filesystem::is_regular_file(current / "PhotonRenderer.info"))
		{
			return Path(current);
		}

		if(!current.has_parent_path())
		{
			break;
		}

		current = current.parent_path();
	}

	return Path();

	// TODO: could do better if we start with module path again as engine may be linked as lib
}

void Filesystem::setInstallationDirectory(const Path& path)
{
	if(s_installationDirectory.has_value())
	{
		throw FilesystemError("Cannot reset installation directory.");
	}

	if(!Filesystem::hasFile(path / "PhotonRenderer.info"))
	{
		throw FilesystemError("Invalid installation directory.");
	}

	s_installationDirectory = path.toCanonical();
}

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

const Path& Filesystem::getInstallationDirectory()
{
	// Cache this as it will not change during runtime
	static auto path =
		[]()
		{
			if(!s_installationDirectory.has_value())
			{
				throw FilesystemError("Installation directory has not been set.");
			}

			return s_installationDirectory.value();
		}();
	return path;
}

const Path& Filesystem::getConfigDirectory()
{
	// Cache this as it will not change during runtime
	static auto path = getInstallationDirectory() / "Config";
	return path;
}

const Path& Filesystem::getScriptDirectory()
{
	// Cache this as it will not change during runtime
	static auto path = getInstallationDirectory() / "Script";
	return path;
}

const Path& Filesystem::getIntermediateDirectory()
{
	// Cache this as it will not change during runtime
	static auto path = getInstallationDirectory() / "Intermediate";
	return path;
}

const Path& Filesystem::getInternalResourceDirectory()
{
	// Cache this as it will not change during runtime
	static auto path = getInstallationDirectory() / "InternalResource";
	return path;
}

const Path& Filesystem::getResourceDirectory()
{
	// Cache this as it will not change during runtime
	static auto path = getInstallationDirectory() / "Photon-v2-Resource";
	return path;
}

}// end namespace ph
