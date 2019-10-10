#pragma once

#include "FileIO/FileSystem/Path.h"

#include <string>

namespace ph
{

class CoreResource
{
public:
	explicit CoreResource(const std::string& identifier);

	Path getPath() const;

private:
	std::string m_identifier;
};

}// end namespace ph
