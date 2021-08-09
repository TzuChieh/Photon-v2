#include "DataIO/SDL/SceneDescription.h"
#include "DataIO/SDL/ETypeCategory.h"
#include "Common/logging.h"

namespace ph
{

PH_DEFINE_INTERNAL_LOG_GROUP(SceneDescription, SDL);

SceneDescription::SceneDescription() :
	m_resources()
{}

void SceneDescription::addResource(
	std::shared_ptr<ISdlResource> resource,
	const std::string&            resourceName)
{
	if(!resource || resourceName.empty())
	{
		const std::string resourceInfo = resource ? sdl::category_to_string(resource->getCategory()) : "no resource";
		const std::string nameInfo     = resourceName.empty() ? resourceName : "no name";

		throw SdlLoadError(
			"cannot add SDL resource due to empty resource/name ("
			"resource: " + resourceInfo + ", "
			"name:" + nameInfo + ")");
	}

	auto& nameToResourceMap = getNameToResourceMap(resource->getCategory());

	const auto& iter = nameToResourceMap.find(resourceName);
	if(iter != nameToResourceMap.end())
	{
		PH_LOG_WARNING(SceneDescription,
			"duplicated SDL resource detected, overwriting (resource: {}, name: {})", 
			sdl::category_to_string(resource->getCategory()), resourceName);
	}

	nameToResourceMap[resourceName] = std::move(resource);
}

std::shared_ptr<ISdlResource> SceneDescription::getResource(
	const std::string&  resourceName,
	const ETypeCategory category) const
{
	const auto& nameToResourceMap = getNameToResourceMap(category);

	const auto& iter = nameToResourceMap.find(resourceName);
	return iter != nameToResourceMap.end() ? iter->second : nullptr;
}

}// end namespace ph
