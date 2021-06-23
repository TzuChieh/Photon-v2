#include "DataIO/SDL/SceneDescription.h"
#include "DataIO/SDL/Keyword.h"
#include "Common/assertion.h"
#include "DataIO/SDL/sdl_exceptions.h"
#include "DataIO/SDL/ETypeCategory.h"
#include "ISdlResource.h"

#include <iostream>

namespace ph
{

namespace
{

const Logger logger(LogSender("Scene Description"));

}

SceneDescription::SceneDescription() :
	m_resources()
{}

void SceneDescription::addResource(
	const SdlTypeInfo& typeInfo,
	const std::string& resourceName, 
	std::unique_ptr<ISdlResource> resource)
{
	if(resource == nullptr || resourceName.empty())
	{
		// TODO: also print type
		std::cerr << "warning: at SceneDescription::addResource(), name <" << resourceName << "> or resource is null, ignoring" << std::endl;
		return;
	}

	const std::size_t categoryIndex = toCategoryIndex(typeInfo.typeCategory);
	auto& resourcesNameMap = m_resources[categoryIndex];
	const auto& iter = resourcesNameMap.find(resourceName);
	if(iter != resourcesNameMap.end())
	{
		std::cerr << "warning: at SceneDescription::addResource(), "
		          << "name <" << resourceName << "> " 
		          << "type <" << typeInfo.toString() << "> duplicated, overwriting" << std::endl;
	}

	resourcesNameMap[resourceName] = std::move(resource);
}

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

	const std::size_t categoryIndex     = toCategoryIndex(resource->getCategory());
	auto&             nameToResourceMap = m_resources[categoryIndex];

	const auto& iter = nameToResourceMap.find(resourceName);
	if(iter != nameToResourceMap.end())
	{
		logger.log(ELogLevel::WARNING_MED, 
			"duplicated SDL resource detected, overwriting ("
			"resource: " + sdl::category_to_string(resource->getCategory()) + ", "
			"name:" + resourceName + ")");
	}

	nameToResourceMap[resourceName] = std::move(resource);
}

std::shared_ptr<ISdlResource> SceneDescription::getResource(
	const std::string&  resourceName,
	const ETypeCategory category) const
{
	const std::size_t categoryIndex     = toCategoryIndex(category);
	const auto&       nameToResourceMap = m_resources[categoryIndex];

	const auto& iter = nameToResourceMap.find(resourceName);
	return iter != nameToResourceMap.end() ? iter->second : nullptr;
}

std::shared_ptr<ISdlResource> SceneDescription::getResource(
	const SdlTypeInfo& typeInfo,
	const std::string& resourceName,
	const DataTreatment& treatment) const
{
	const std::size_t categoryIndex = toCategoryIndex(typeInfo.typeCategory);
	const auto& resourcesNameMap = m_resources[categoryIndex];
	const auto& iter = resourcesNameMap.find(resourceName);
	if(iter == resourcesNameMap.end())
	{
		reportResourceNotFound(SdlTypeInfo::categoryToName(typeInfo.typeCategory), resourceName, treatment);
		return nullptr;
	}

	return iter->second;
}

std::vector<std::shared_ptr<Actor>> SceneDescription::getActors() const
{
	std::vector<std::shared_ptr<Actor>> actors;

	const std::size_t actorCategoryIndex = static_cast<std::size_t>(Actor::ciTypeInfo().typeCategory);
	for(auto& keyValuePair : m_resources[actorCategoryIndex])
	{
		const std::shared_ptr<Actor> actor = std::dynamic_pointer_cast<Actor>(keyValuePair.second);
		if(actor != nullptr)
		{
			actors.push_back(actor);
		}
		else
		{
			std::cerr << "warning: at SceneDescription::getActors(), non-Actor detected" << std::endl;
		}
	}

	return actors;
}

void SceneDescription::reportResourceNotFound(const std::string& categoryName, const std::string& name, const DataTreatment& treatment)
{
	const std::string& message = treatment.notFoundInfo;

	switch(treatment.importance)
	{
	case EDataImportance::OPTIONAL:
		if(!message.empty())
		{
			std::cerr << "warning: optional resource category<" << categoryName << "> name<" << name << "> not found (" << message << ")" << std::endl;
		}
		break;

	case EDataImportance::REQUIRED:
		std::cerr << "warning: required resource category<" << categoryName << "> name<" << name << "> not found";
		if(!message.empty())
		{
			std::cerr << " (" << message << ")";
		}
		std::cerr << std::endl;
		break;
	}
}

std::size_t SceneDescription::toCategoryIndex(const ETypeCategory category) const
{
	const std::size_t index = static_cast<std::size_t>(category);
	PH_ASSERT(index < m_resources.size());

	return index;
}

}// end namespace ph
