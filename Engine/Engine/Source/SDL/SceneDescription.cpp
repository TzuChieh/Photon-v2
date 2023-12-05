#include "SDL/SceneDescription.h"

#include <Common/logging.h>

#include <utility>

namespace ph
{

PH_DEFINE_INTERNAL_LOG_GROUP(SceneDescription, SDL);

SceneDescription::SceneDescription()
	: ISdlReferenceGroup()
	, m_workingDirectory()
	, m_resources()
	, m_phantomResources()
{}

std::shared_ptr<ISdlResource> SceneDescription::get(std::string_view resourceName) const
{
	std::shared_ptr<ISdlResource> result = m_resources.get(resourceName);
	if(!result)
	{
		result = m_phantomResources.get(resourceName);
	}

	return result;
}

bool SceneDescription::has(std::string_view resourceName) const
{
	if(m_resources.has(resourceName))
	{
		return true;
	}
	
	if(m_phantomResources.has(resourceName))
	{
		return true;
	}

	return false;
}

void SceneDescription::setWorkingDirectory(Path directory)
{
	m_workingDirectory = std::move(directory);
}

}// end namespace ph
