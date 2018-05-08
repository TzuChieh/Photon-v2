#include "FileIO/SDL/NamedResourceStorage.h"
#include "FileIO/SDL/Keyword.h"

#include <iostream>

namespace ph
{

NamedResourceStorage::NamedResourceStorage() : 
	m_resources(static_cast<std::size_t>(ETypeCategory::MAX) + 1)
{}

void NamedResourceStorage::addResource(
	const SdlTypeInfo& typeInfo,
	const std::string& resourceName, 
	std::unique_ptr<ISdlResource> resource)
{
	if(resource == nullptr || resourceName.empty())
	{
		std::cerr << "warning: at NamedResourceStorage::addResource(), name <" << resourceName << "> or resource is null, ignoring" << std::endl;
		return;
	}

	const std::size_t categoryIndex = static_cast<std::size_t>(typeInfo.typeCategory);
	if(!checkCategoryIndex(categoryIndex))
	{
		return;
	}

	auto& resourcesNameMap = m_resources[categoryIndex];
	const auto& iter = resourcesNameMap.find(resourceName);
	if(iter != resourcesNameMap.end())
	{
		std::cerr << "warning: at NamedResourceStorage::addResource(), name <" << resourceName << "> duplicated, overwriting" << std::endl;
	}

	resourcesNameMap[resourceName] = std::move(resource);
}

std::shared_ptr<ISdlResource> NamedResourceStorage::getResource(
	const SdlTypeInfo& typeInfo,
	const std::string& resourceName,
	const DataTreatment& treatment) const
{
	const std::size_t categoryIndex = static_cast<std::size_t>(typeInfo.typeCategory);
	if(!checkCategoryIndex(categoryIndex))
	{
		return nullptr;
	}

	const auto& resourcesNameMap = m_resources[categoryIndex];
	const auto& iter = resourcesNameMap.find(resourceName);
	if(iter == resourcesNameMap.end())
	{
		reportResourceNotFound(SdlTypeInfo::categoryToName(typeInfo.typeCategory), resourceName, treatment);
		return nullptr;
	}

	return iter->second;
}

std::vector<std::shared_ptr<Actor>> NamedResourceStorage::getActors() const
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
			std::cerr << "warning: at NamedResourceStorage::getActors(), non-Actor detected" << std::endl;
		}
	}

	return actors;
}

void NamedResourceStorage::reportResourceNotFound(const std::string& categoryName, const std::string& name, const DataTreatment& treatment)
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

bool NamedResourceStorage::checkCategoryIndex(const std::size_t index)
{
	if(index >= (static_cast<std::size_t>(ETypeCategory::MAX) + 1))
	{
		std::cerr << "warning: at NamedResourceStorage::checkCategoryIndex(), category index overflow" << std::endl;
		return false;
	}

	return true;
}

}// end namespace ph