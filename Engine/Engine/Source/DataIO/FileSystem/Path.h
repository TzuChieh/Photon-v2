#pragma once

#include "Utility/TSpan.h"

#include <Utility/string_utils.h>
#include <Common/compiler.h>

#include <cstddef>
#include <string>
#include <utility>

// TODO: other platforms and versions that do not need the "experimental" folder
// NOTE: g++ 8.0 supports filesystem finally
#if PH_COMPILER_IS_MSVC

	#include <filesystem>
	namespace std_filesystem = std::filesystem;

#elif PH_COMPILER_IS_GCC

	#include <experimental/filesystem>
	namespace std_filesystem = std::experimental::filesystem;

#else

	/*
		Assuming OSX here.
		Since OSX has no support for filesystem library before Xcode 10.1, we use
		an alternative Path implementation that does not depend on STL's filesystem.
	*/
	#define PH_USE_ALTERNATIVE_PATH_IMPL

#endif

#ifndef PH_USE_ALTERNATIVE_PATH_IMPL

namespace ph
{

/*! @brief General path representation.
Does not check whether the target actually exists (e.g., on a filesystem). To check target existence,
use `Filesystem`.
*/
class Path final
{
public:
	/*! @brief Creates empty path.
	*/
	Path();
	
	/*!
	Constructs a path from some string representation of the path. The string can be either 
	a relative or absolute path, or even a path fragment. The input format of `path` should be in
	a general, "mostly" OS neutral form [1] which often referred as "generic format" in many libraries.
	
	References:
	[1] https://www.boost.org/doc/libs/1_83_0/libs/filesystem/doc/reference.html
	*/
	explicit Path(std::string path);

	/*!
	A `std::string_view` variant of Path(std::string).
	*/
	explicit Path(std::string_view path);

	/*!
	A span variant of Path(std::string).
	*/
	explicit Path(TSpanView<char> path);

	/*!
	Similar to Path(std::string), except the string is a null-terminated character sequence pointed
	to by `path`.
	*/
	explicit Path(const char* path);

	explicit Path(std::filesystem::path path);

	bool isRelative() const;
	bool isAbsolute() const;
	bool isEmpty() const;
	void clear();
	
	/*!
	Appending one path to another. System specific directory separators are added in 
	between two path objects.
	*/
	Path append(const Path& other) const;

	Path append(std::string_view pathStr) const;

	Path toAbsolute() const;

	/*! @brief Get a string representation of this path in generic format.
	*/
	std::string toString() const;

	std::string toAbsoluteString() const;

	/*! @brief Get a string representation of this path in native format.
	@return A string that can be passed to the native OS for identifying a target.
	*/
	std::string toNativeString() const;

	/*! @brief Similar to `toNativeString()`, except the buffer is provided.
	This variant guarantees no dynamic allocation takes place.
	@param out_buffer The buffer to store the string.
	@param out_numTotalChars Total number of characters in the string representation of this path
	(including the null terminator).
	@param isNullTerminated Whether the resultant string should be null-terminated. If true,
	null terminator will present even if the string is not fully copied.
	@return Number of copied characters (including the null terminator).
	*/
	std::size_t toNativeString(
		TSpan<char> out_buffer, 
		std::size_t* out_numTotalChars = nullptr,
		bool isNullTerminated = true) const;

	/*! @brief Get a standard path representation of this path.
	*/
	std::filesystem::path toStdPath() const;

	Path removeLeadingSeparator() const;

	Path removeTrailingSeparator() const;

	/*! @brief Returns the filename if present.
	@return Filename including the extension part (if any). Empty if not present.
	*/
	std::string getFilename() const;

	/*! @brief Returns filename extension if present.
	The extension string will start with a period character ".". If the path
	contains no extension, then an empty string is returned.
	*/
	std::string getExtension() const;

	/*!
	If the path starts with a root directory specifier, it will be returned. Example: for the path 
	"/abc/def/", "/" will be returned.
	*/
	Path getLeadingElement() const;

	/*!
	If the path ends with a path separator, the element returned will be the name before the separator. 
	For example, "C:\\abc\\def\\ghi\\" will return "ghi" as the trailing element. If this is not desired
	and an empty path "" is expected instread, set @p ignoreTrailingSeparator to `false`. Filename
	with its extension is considered a single element, i.e., "./aaa/bbb.ext" would return "bbb.ext".
	*/
	Path getTrailingElement(bool ignoreTrailingSeparator = true) const;

	/*!
	Get the parent directory of this path, e.g., the parent of "./aaa/bbb.ext" would be "./aaa".
	*/
	Path getParent() const;

	// TODO: replace/remove filename

	/*! @brief Changes filename extension in the path, if any.
	The behavior is the same as `std::filesystem::path::replace_extension(1)`.
	*/
	Path replaceExtension(std::string_view replacement) const;

	/*! @brief Removes filename extension in the path, if any.
	*/
	Path removeExtension() const;

	Path operator / (const Path& other) const;
	Path operator / (std::string_view pathStr) const;
	bool operator == (const Path& other) const;

private:
	std::filesystem::path m_path;

	static wchar_t charToWchar(const char ch);
};

inline bool Path::isRelative() const
{
	return m_path.is_relative();
}

inline bool Path::isAbsolute() const
{
	return m_path.is_absolute();
}

inline bool Path::isEmpty() const
{
	return m_path.empty();
}

inline void Path::clear()
{
	m_path.clear();
}

inline std::filesystem::path Path::toStdPath() const
{
	return m_path;
}

}// end namespace ph

PH_DEFINE_INLINE_TO_STRING_FORMATTER(ph::Path);

#else

#include "DataIO/FileSystem/AltPath.h"

namespace ph
{

using Path = AltPath;

}// end namespace ph
	
#endif
