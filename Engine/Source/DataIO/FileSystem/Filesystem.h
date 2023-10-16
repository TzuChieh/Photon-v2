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

	/*! @brief Copy a file to another destination.
	Both paths should represent a file.
	@param overwriteExisting If `true`, the existing file `dstFile` will be overwritten by the content
	from `srcFile`.
	@exception FilesystemError If any error during copying occurred.
	*/
	static void copyFile(
		const Path& srcFile, 
		const Path& dstFile, 
		bool overwriteExisting = false);

	/*! @brief Copy a directory and its content recursively to another destination.
	Both paths should represent a directory.
	@param overwriteExisting If `true`, any existing file will be overwritten.
	@exception FilesystemError If any error during copying occurred.
	*/
	static void copyDirectories(
		const Path& srcDir, 
		const Path& dstDir,
		bool overwriteExisting = false);

	static Path makeRelative(const Path& src, const Path& base);
};

}// end namespace ph
