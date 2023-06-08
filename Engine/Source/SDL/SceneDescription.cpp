#include "SDL/SceneDescription.h"
#include "Common/logging.h"

#include <utility>

namespace ph
{

PH_DEFINE_INTERNAL_LOG_GROUP(SceneDescription, SDL);

SceneDescription::SceneDescription()
	: ISdlReferenceGroup()
	, m_workingDirectory("./temp_sdl/")
	, m_resources()
	, m_phantomResources()
{}

std::shared_ptr<ISdlResource> SceneDescription::get(
	std::string_view resourceName,
	const ESdlTypeCategory category) const
{
	std::shared_ptr<ISdlResource> result = m_resources.get(resourceName, category);
	if(!result)
	{
		result = m_phantomResources.get(resourceName, category);
	}

	return result;
}

bool SceneDescription::has(
	std::string_view resourceName,
	const ESdlTypeCategory category) const
{
	if(m_resources.has(resourceName, category))
	{
		return true;
	}
	
	if(m_phantomResources.has(resourceName, category))
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
