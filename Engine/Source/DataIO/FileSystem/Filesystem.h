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
	@param overwriteExisting If `true`, the existing file `dstFile` (if present) will be overwritten
	by the content from `srcFile`.
	@exception FilesystemError If any error during copying occurred.
	*/
	static void copyFile(
		const Path& srcFile, 
		const Path& dstFile, 
		bool overwriteExisting = false);

	static void copyFileToDirectory(
		const Path& srcFile,
		const Path& dstDir,
		bool overwriteExisting = false,
		bool createMissingDirectories = true);

	/*! @brief Copy a file or directory recursively to another destination.
	If `srcPath` is a file, `dstPath` will be interpreted as a file and the method is equivalent to
	calling `copyFile()`. If `srcPath` is a directory, `dstPath` will be interpreted as a directory.
	For directory copying, contents in `srcPath` will be copied to `dstPath`. Missing directories
	are automatically created.
	@param overwriteExisting If `true`, any existing file will be overwritten.
	@exception FilesystemError If any error during copying occurred.
	*/
	static void copy(
		const Path& srcPath, 
		const Path& dstPath,
		bool overwriteExisting = false);

	static Path makeRelative(const Path& src, const Path& base);
};

}// end namespace ph
