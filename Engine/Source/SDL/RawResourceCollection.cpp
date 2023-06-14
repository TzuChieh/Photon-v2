#include "SDL/RawResourceCollection.h"
#include "Common/logging.h"
#include "SDL/sdl_helpers.h"

namespace ph
{

PH_DEFINE_INTERNAL_LOG_GROUP(RawResourceCollection, SDL);

RawResourceCollection::RawResourceCollection()
	: ISdlReferenceGroup()
	, m_nameToResource()
{}

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

	const auto& iter = m_nameToResource.find(resourceName);
	if(iter != m_nameToResource.end())
	{
		PH_LOG_WARNING(RawResourceCollection,
			"duplicated SDL resource detected, overwriting (resource: {}, name: {})", 
			sdl::category_to_string(resource->getDynamicCategory()), resourceName);
	}

	m_nameToResource[std::string(resourceName)] = std::move(resource);
}

void RawResourceCollection::listAll(
	std::vector<const ISdlResource*>& out_resources,
	std::vector<std::string_view>* const out_resourceNames) const
{
	for(const auto& keyValPair : m_nameToResource)
	{
		out_resources.push_back(keyValPair.second.get());

		if(out_resourceNames)
		{
			out_resourceNames->push_back(keyValPair.first);
		}
	}
}

}// end namespace ph
