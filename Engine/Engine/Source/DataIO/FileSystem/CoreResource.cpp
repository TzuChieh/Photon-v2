#include "DataIO/FileSystem/CoreResource.h"

#include <Common/config.h>

namespace ph
{

CoreResource::CoreResource(const std::string& identifier) : 
	m_identifier(identifier)
{}

Path CoreResource::getPath() const
{
	const Path& baseDirectory = Path(Config::RENDERER_RESOURCE_DIRECTORY());
	return baseDirectory.append(Path(m_identifier));
}

}// end namespace ph
