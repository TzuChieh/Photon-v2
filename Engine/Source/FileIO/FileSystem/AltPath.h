#pragma once

#include "Common/ThirdParty/lib_stb.h"
#include "Common/assertion.h"

#include <string>
#include <vector>
#include <iostream>

namespace ph
{

/*
	This path class has exactly the same interface as FileIO/FileSystem/Path.h,
	except that its implementation does not depend on STL.
*/
class AltPath
{
public:
	inline AltPath() :
		AltPath("./")
	{}

	// Constructing a path from some string representing of the path. The string 
	// can be either a relative or absolute path, or even a path fragment. The
	// constructed path substitutes all separators to a system specific (preferred)
	// one.
	//
	inline explicit AltPath(const std::string& path) :
		m_path(path.begin(), path.end())
	{
		m_path.push_back('\0');
		stb_fixpath(m_path.data());
	}

	inline bool isRelative() const
	{
		return !isAbsolute();
	}

	inline bool isAbsolute() const
	{
		return !m_path.empty() && (m_path[0] == '/' || m_path[0] == '~');
	}

	inline std::string toAbsoluteString() const
	{
		std::vector<char> absolutePath(m_path.size() + 1024);
		if(stb_fullpath(
			absolutePath.data(), 
			absolutePath.size(), 
			std::vector<char>(m_path).data()) == STB_TRUE)
		{
			return std::string(absolutePath.data());
		}
		else
		{
			std::cerr << "warning: at StbPath::getAbsoluteString(), " 
			          << "path <" 
			          << std::string(absolutePath.data())
			          << "> cannot convert to absolute path" << std::endl;
			return std::string();
		}
	}

	// Appending one path to another. System specific directory separators are
	// added in between two path objects.
	//
	inline AltPath append(const AltPath& other) const
	{
		auto thisPath  = this->removeTrailingSeparator();
		auto otherPath = other.removeLeadingSeparator();

		PH_ASSERT_GT(thisPath.m_path.size(),  0);
		PH_ASSERT_GT(otherPath.m_path.size(), 0);

		if(thisPath.m_path.size() <= 1)
		{
			return thisPath;
		}

		std::vector<char> resultPath(thisPath.m_path);
		resultPath.back() = '/';
		resultPath.insert(resultPath.end(), otherPath.m_path.begin(), otherPath.m_path.end());

		return AltPath(resultPath.data());
	}

	inline std::string toString() const
	{
		return std::string(m_path.data());
	}

	inline AltPath removeLeadingSeparator() const
	{
		std::vector<char> resultPath(m_path);
		while(!resultPath.empty())
		{
			if(resultPath.front() == '/' || resultPath.front() == '\\')
			{
				resultPath.erase(resultPath.begin());
			}
			else
			{
				break;
			}
		}

		return AltPath(std::string(resultPath.data()));
	}

	inline AltPath removeTrailingSeparator() const
	{
		std::vector<char> resultPath(m_path);
		while(resultPath.size() >= 2)
		{
			const char lastChar = resultPath[resultPath.size() - 2];
			if(lastChar == '/' || lastChar == '\\')
			{
				resultPath.pop_back();
				resultPath.back() = '\0';
			}
			else
			{
				break;
			}
		}

		return AltPath(std::string(resultPath.data()));
	}

	// Returns filename extension if present. The extension string will 
	// start with a period character ".". If the path contains no extension,
	// then an empty string is returned.
	//
	inline std::string getExtension() const
	{
		std::string path(m_path.data());
		const std::size_t dotIndex = path.find_last_of('.');
		if(dotIndex != std::string::npos)
		{
			return path.substr(dotIndex);
		}
		else
		{
			return std::string();
		}
	}

	inline bool operator == (const AltPath& other) const
	{
		return m_path == other.m_path;
	}

private:
	std::vector<char> m_path;
};

}// end namespace ph