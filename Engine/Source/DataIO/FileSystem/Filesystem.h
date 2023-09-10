#pragma once

namespace ph
{

class Path;

class Filesystem final
{
public:
	/*! @brief Check if the path points to an existing directory.
	*/
	static bool hasDirectory(const Path& path);

	/*! @brief Check if the path points to an existing file.
	*/
	static bool hasFile(const Path& path);

	/*! @brief Create a directory as specified by the path.
	Similar to `createDirectories()`, except that the parent directory must already exist.
	*/
	static void createDirectory(const Path& path);

	/*! @brief Create a directory as specified by the path.
	Treat the path as a directory representation and create all missing folders if any of them
	does not already exist.
	*/
	static void createDirectories(const Path& path);

	static Path makeRelative(const Path& src, const Path& base);
};

}// end namespace ph
