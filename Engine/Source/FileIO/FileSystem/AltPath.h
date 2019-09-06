#pragma once

#include "Common/assertion.h"
#include "Common/os.h"

#include <string>
#include <iostream>

#ifdef PH_OPERATING_SYSTEM_IS_WINDOWS
	#include <direct.h>
#else
	#include <unistd.h>
#endif

namespace ph
{

/*
	This path class has exactly the same interface as FileIO/FileSystem/Path.h,
	except that its implementation does not depend on STL's filesystem.
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
		m_path(path)
	{
		for(char& ch : m_path)
		{
			if(ch == '\\')
			{
				ch = '/';
			}
		}
	}

	inline bool isRelative() const
	{
		return !isAbsolute();
	}

	inline bool isAbsolute() const
	{
		if(m_path.empty())
		{
			return false;
		}
		else
		{
		#ifdef PH_OPERATING_SYSTEM_IS_WINDOWS
			return m_path.length() >= 3 && m_path[1] == ':' && m_path[2] == '/';
		#else
			return m_path[0] == '/' || m_path[0] == '~';
		#endif
		}

		return !m_path.empty() && (m_path[0] == '/' || m_path[0] == '~');
	}

	inline bool isEmpty() const
	{
		return m_path.empty();
	}

	inline std::string toAbsoluteString() const
	{
		if(isAbsolute())
		{
			return m_path;
		}

		constexpr std::size_t BUFFER_SIZE = 1024;
		std::string buffer(BUFFER_SIZE, '\0');
		AltPath workingDirectory;
	#ifdef _WIN32
		workingDirectory = AltPath(std::string(_getcwd(buffer.data(), BUFFER_SIZE)));
	#else
		workingDirectory = AltPath(std::string(getcwd(buffer.data(), BUFFER_SIZE)));
	#endif

		workingDirectory = workingDirectory.removeTrailingSeparator();

		return workingDirectory.append(this->removeLeadingSeparator()).toString();
	}

	// Appending one path to another. System specific directory separators are
	// added in between two path objects.
	//
	inline AltPath append(const AltPath& other) const
	{
		const auto thisPath  = this->removeTrailingSeparator().m_path;
		const auto otherPath = other.removeLeadingSeparator().m_path;

		return AltPath(thisPath + '/' + otherPath);
	}

	inline std::string toString() const
	{
		return std::string(m_path.data());
	}

	inline AltPath removeLeadingSeparator() const
	{
		std::string resultPath(m_path);
		while(!resultPath.empty())
		{
			if(resultPath.front() == '/')
			{
				resultPath.erase(resultPath.begin());
			}
			else
			{
				break;
			}
		}

		return AltPath(resultPath);
	}

	inline AltPath removeTrailingSeparator() const
	{
		std::string resultPath(m_path);
		while(!resultPath.empty())
		{
			if(resultPath.back() == '/')
			{
				resultPath.pop_back();
			}
			else
			{
				break;
			}
		}

		return AltPath(resultPath);
	}

	// Returns filename extension if present. The extension string will 
	// start with a period character ".". If the path contains no extension,
	// then an empty string is returned.
	//
	inline std::string getExtension() const
	{
		const std::size_t dotIndex = m_path.find_last_of('.');
		if(dotIndex != std::string::npos)
		{
			return m_path.substr(dotIndex);
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
	std::string m_path;
};

}// end namespace ph