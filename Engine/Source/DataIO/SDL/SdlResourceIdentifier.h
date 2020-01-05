#pragma once

#include "DataIO/FileSystem/Path.h"

#include <string>

namespace ph
{

class SdlResourceIdentifier final
{
public:
	inline SdlResourceIdentifier(const std::string& identifier, 
	                             const Path&        workingDirectory) : 
		m_identifier(identifier), 
		m_workingDirectory(workingDirectory)
	{}

	inline Path getPathToResource() const
	{
		return m_workingDirectory.append(Path(m_identifier));
	}

private:
	std::string m_identifier;
	Path        m_workingDirectory;
};

}// end namespace ph
