#pragma once

#include <filesystem>
#include <string>
#include <iostream>

namespace ph
{

class Path final
{
public:
	inline Path() : 
		Path(std::experimental::filesystem::current_path().string())
	{

	}

	inline explicit Path(const std::string& path) : 
		m_path(path)
	{

	}

	inline bool isRelative() const
	{
		return m_path.is_relative();
	}

	inline bool isAbsolute() const
	{
		return m_path.is_absolute();
	}

	inline std::string toAbsoluteString() const
	{
		const std::string& absPath = std::experimental::filesystem::absolute(m_path).string();
		if(!Path(absPath).isAbsolute())
		{
			std::cerr << "warning: at Path::getAbsoluteString(), " 
			          << "path <" << m_path.string() << "> cannot convert to absolute path" << std::endl;
		}

		return absPath;
	}

	inline Path append(const Path& other) const
	{
		auto thisPath  = m_path;
		auto otherPath = other.m_path;
		return Path(thisPath.append(otherPath).string());
	}

private:
	std::experimental::filesystem::path m_path;
};

}// end namespace ph