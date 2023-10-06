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

std::shared_ptr<ISdlResource> RawResourceCollection::remove(std::string_view resourceName)
{
	auto iter = m_nameToResource.find(resourceName);
	if(iter == m_nameToResource.end())
	{
		return nullptr;
	}

	std::shared_ptr<ISdlResource> removedResource = iter->second;
	m_nameToResource.erase(iter);
	return removedResource;
}

std::string RawResourceCollection::rename(std::string_view resourceName, std::string_view newResourceName)
{
	std::shared_ptr<ISdlResource> resource = remove(resourceName);
	if(!resource)
	{
		throw_formatted<SdlLoadError>(
			"failed to rename SDL resource \"{}\": resource not found", resourceName);
	}

	std::string finalName = makeResourceName(newResourceName);
	add(std::move(resource), finalName);

	return finalName;
}

std::string RawResourceCollection::makeResourceName(std::string_view intendedName)
{
	int suffixNumber = 1;
	while(true)
	{
		// Generating a name sequence like "name", "name_2", "name_3", etc.
		std::string generatedName = 
			std::string(intendedName) +
			(suffixNumber == 1 ? "" : "_" + std::to_string(suffixNumber));

		if(!has(generatedName))
		{
			return generatedName;
		}

		++suffixNumber;
	}

	PH_ASSERT_UNREACHABLE_SECTION();
	return "";
}

std::vector<const ISdlResource*> RawResourceCollection::listAll(
	std::vector<std::string>* const out_resourceNames) const
{
	std::vector<const ISdlResource*> resources;
	for(const auto& [name, resource] : m_nameToResource)
	{
		resources.push_back(resource.get());

		if(out_resourceNames)
		{
			out_resourceNames->push_back(name);
		}
	}
	return resources;
}

}// end namespace ph
