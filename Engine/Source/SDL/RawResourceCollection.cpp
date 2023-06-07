#include "SDL/RawResourceCollection.h"
#include "Common/logging.h"

namespace ph
{

PH_DEFINE_INTERNAL_LOG_GROUP(RawResourceCollection, SDL);

RawResourceCollection::RawResourceCollection()
	: ISdlReferenceGroup()
	, m_resources()
{}

std::shared_ptr<ISdlResource> RawResourceCollection::get(
	std::string_view resourceName,
	const ESdlTypeCategory category) const
{
	const auto& nameToResourceMap = getNameToResourceMap(category);

	const auto& iter = nameToResourceMap.find(resourceName);
	return iter != nameToResourceMap.end() ? iter->second : nullptr;
}

void RawResourceCollection::add(
	std::shared_ptr<ISdlResource> resource,
	std::string_view resourceName)
{
	if(!resource || resourceName.empty())
	{
		throw_formatted<SdlLoadError>(
			"cannot add SDL resource due to empty resource/name (resource: {}, name: {})", 
			resource ? sdl::category_to_string(resource->getDynamicCategory()) : "no resource",
			resourceName.empty() ? resourceName : "no name");
	}

	auto& nameToResourceMap = getNameToResourceMap(resource->getDynamicCategory());

	const auto& iter = nameToResourceMap.find(resourceName);
	if(iter != nameToResourceMap.end())
	{
		PH_LOG_WARNING(RawResourceCollection,
			"duplicated SDL resource detected, overwriting (resource: {}, name: {})", 
			sdl::category_to_string(resource->getDynamicCategory()), resourceName);
	}

	nameToResourceMap[std::string(resourceName)] = std::move(resource);
}

void RawResourceCollection::listAll(
	std::vector<const ISdlResource*>* const out_resources,
	std::vector<std::string_view>* const out_resourceNames) const
{
	PH_ASSERT(out_resources);

	out_resources->clear();
	for(const auto& nameToResource : m_resources)
	{
		for(const auto& keyValPair : nameToResource)
		{
			out_resources->push_back(keyValPair.second.get());

			if(out_resourceNames)
			{
				out_resourceNames->push_back(keyValPair.first);
			}
		}
	}
}

}// end namespace ph
