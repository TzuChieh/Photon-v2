#pragma once

#include "Common/compiler.h"
#include "Utility/string_utils.h"
#include "Utility/TSpan.h"

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

class Path final
{
public:
	/*! @brief Creates empty path.
	*/
	inline Path() : 
		m_path()
	{}
	
	/*!
	Constructs a path from some string representation of the path. The string can be either 
	a relative or absolute path, or even a path fragment. The constructed path substitutes 
	all separators to a system specific (preferred) one.
	*/
	inline explicit Path(std::string path) : 
		m_path(std_filesystem::path(std::move(path)))
	{
		m_path.make_preferred();
	}

	/*!
	A std::string_view variant of Path(std::string).
	*/
	inline explicit Path(const std::string_view path) :
		m_path(std_filesystem::path(path))
	{
		m_path.make_preferred();
	}

	/*!
	Similar to Path(std::string), except the string is a null-terminated character sequence pointed to
	by @p path.
	*/
	inline explicit Path(const char* const path) :
		Path(std::string_view(path))
	{}

	inline explicit Path(std_filesystem::path path) :
		m_path(std::move(path))
	{
		m_path.make_preferred();
	}

	inline bool isRelative() const
	{
		return m_path.is_relative();
	}

	inline bool isAbsolute() const
	{
		return m_path.is_absolute();
	}

	bool isEmpty() const;
	void clear();
	
	/*!
	Appending one path to another. System specific directory separators are added in 
	between two path objects.
	*/
	inline Path append(const Path& other) const
	{
		auto thisPath = this->removeTrailingSeparator();
		auto otherPath = other.removeLeadingSeparator();
		return Path(thisPath.m_path / otherPath.m_path);

		// FIXME: strangely this fails unit test; windows with slash prefix = root?
		//return Path(this->m_path / other.m_path);
	}

	inline Path append(std::string_view pathStr) const
	{
		return append(Path(pathStr));
	}

	/*! @brief Get a string representation of this path.
	*/
	std::string toString() const;

	/*! @brief Similar to `toString()`, except the buffer is provided.
	This variant guarantees no dynamic allocation takes place.
	@param out_buffer The buffer to store the string.
	@param out_numTotalChars Total number of characters in the string representation of this path
	(including the null terminator).
	@param isNullTerminated Whether the resultant string should be null-terminated. If true,
	null terminator will present even if the string is not fully copied.
	@return Number of copied characters (including the null terminator).
	*/
	std::size_t toString(
		TSpan<char> out_buffer, 
		std::size_t* out_numTotalChars = nullptr,
		bool isNullTerminated = true) const;

	std::string toAbsoluteString() const;

	/*! @brief Get a standard path representation of this path.
	*/
	std_filesystem::path toStdPath() const;

	inline Path removeLeadingSeparator() const
	{
		std::string pathStr = m_path.string();
		while(!pathStr.empty())
		{
			if(charToWchar(pathStr.front()) == m_path.preferred_separator)
			{
				pathStr.erase(0, 1);
			}
			else
			{
				break;
			}
		}

		return Path(pathStr);
	}

	inline Path removeTrailingSeparator() const
	{
		std::string pathStr = m_path.string();
		while(!pathStr.empty())
		{
			if(charToWchar(pathStr.back()) == m_path.preferred_separator)
			{
				pathStr.pop_back();
			}
			else
			{
				break;
			}
		}

		return Path(pathStr);
	}

	/*! @brief Returns the filename if present.
	@return Filename including the extension part. Empty if not present.
	*/
	std::string getFilename() const;

	/*! @brief Returns filename extension if present.
	The extension string will start with a period character ".". If the path
	contains no extension, then an empty string is returned.
	*/
	std::string getExtension() const;

	/*! @brief Check if the path points to an existing directory.
	*/
	bool hasDirectory() const;

	/*! @brief Check if the path points to an existing file.
	*/
	bool hasFile() const;

	// TODO: isDirectory() and isFile(), they should only analyze the path string, 
	// not checking if actual resource exist

	/*! @brief Create a directory as specified by the path.
	Treat the path as a directory representation and create all missing folders
	if any of them does not already exist.
	*/
	void createDirectory() const;

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
	std_filesystem::path m_path;

	static wchar_t charToWchar(const char ch);
};

inline bool Path::isEmpty() const
{
	return m_path.empty();
}

inline void Path::clear()
{
	m_path.clear();
}

inline std::string Path::getFilename() const
{
	return m_path.filename().string();
}

inline std::string Path::getExtension() const
{
	return m_path.extension().string();
}

inline bool Path::hasDirectory() const
{
	return std::filesystem::is_directory(m_path);
}

inline bool Path::hasFile() const
{
	return std::filesystem::is_regular_file(m_path);
}

inline void Path::createDirectory() const
{
	std::filesystem::create_directories(m_path);
}

inline std::string Path::toString() const
{
	return m_path.string();
}

inline std_filesystem::path Path::toStdPath() const
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
