#pragma once

#include "Common/compiler.h"

#include <string>
#include <iostream>
#include <cwchar>

// TODO: other platforms and versions that do not need the "experimental" folder
// NOTE: g++ 8.0 supports filesystem finally
#if defined(PH_COMPILER_IS_MSVC)

	#include <filesystem>
	namespace std_filesystem = std::filesystem;

#elif defined(PH_COMPILER_IS_GCC)

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

class Path
{
public:
	inline Path() : 
		Path(std_filesystem::current_path().string())
	{}

	// Constructing a path from some string representation of the path. The 
	// string can be either a relative or absolute path, or even a path 
	// fragment. The constructed path substitutes all separators to a system 
	// specific (preferred) one.
	//
	inline explicit Path(const std::string& path) : 
		m_path(std_filesystem::path(path).make_preferred())
	{}

	inline bool isRelative() const
	{
		return m_path.is_relative();
	}

	inline bool isAbsolute() const
	{
		return m_path.is_absolute();
	}

	inline bool isEmpty() const
	{
		return m_path.empty();
	}

	inline std::string toAbsoluteString() const
	{
		const std::string& absPath = std_filesystem::absolute(m_path).string();
		if(!Path(absPath).isAbsolute())
		{
			std::cerr << "warning: at Path::getAbsoluteString(), " 
			          << "path <" 
			          << m_path.string() 
			          << "> cannot convert to absolute path" << std::endl;
		}

		return absPath;
	}

	// Appending one path to another. System specific directory separators are
	// added in between two path objects.
	//
	inline Path append(const Path& other) const
	{
		auto thisPath  = this->removeTrailingSeparator();
		auto otherPath = other.removeLeadingSeparator();
		return Path((thisPath.m_path /= otherPath.m_path).string());
	}

	inline std::string toString() const
	{
		return m_path.string();
	}

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

	/*! @brief Returns filename extension if present.

	The extension string will start with a period character ".". If the path
	contains no extension, then an empty string is returned.
	*/
	inline std::string getExtension() const
	{
		return m_path.extension().string();
	}

	bool isDirectory() const;
	bool isFile() const;

	inline bool operator == (const Path& other) const
	{
		return m_path == other.m_path;
	}

private:
	std_filesystem::path m_path;

	inline static wchar_t charToWchar(const char ch)
	{
		const std::wint_t wch = std::btowc(ch);
		if(wch == WEOF)
		{
			std::cout << "warning: at Path::charToWchar(), " 
			          << "char <" << ch
			          << "> failed to widen to wchar" << std::endl;
		}
		
		return static_cast<wchar_t>(wch);
	}
};

// In-header Implementations:

inline bool Path::isDirectory() const
{
	return std::filesystem::is_directory(m_path);
}

inline bool Path::isFile() const
{
	return std::filesystem::is_regular_file(m_path);
}

}// end namespace ph

#else

#include "DataIO/FileSystem/AltPath.h"

namespace ph
{

using Path = AltPath;

}// end namespace ph
	
#endif
