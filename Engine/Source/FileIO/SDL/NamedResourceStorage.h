#pragma once

#include "Actor/AModel.h"
#include "Actor/ALight.h"
#include "FileIO/SDL/DataTreatment.h"
#include "FileIO/SDL/SdlTypeInfo.h"
#include "FileIO/SDL/ISdlResource.h"

#include <unordered_map>
#include <string>
#include <memory>
#include <vector>
#include <iostream>
#include <array>

namespace ph
{

class NamedResourceStorage final
{
public:
	NamedResourceStorage();

	void addResource(
		const SdlTypeInfo& typeInfo,
		const std::string& resourceName,
		std::unique_ptr<ISdlResource> resource);

	std::shared_ptr<ISdlResource> getResource(
		const SdlTypeInfo& typeInfo,
		const std::string& resourceName,
		const DataTreatment& treatment = DataTreatment()) const;

	template<typename T>
	std::shared_ptr<T> getResource(
		const std::string& resourceName, 
		const DataTreatment& treatment = DataTreatment()) const;

	template<typename T>
	bool hasResource(const std::string& resourceName) const;

	std::vector<std::shared_ptr<Actor>> getActors() const;

private:
	std::array<
		std::unordered_map<std::string, std::shared_ptr<ISdlResource>>, 
		static_cast<std::size_t>(ETypeCategory::MAX)
	> m_resources;
	
private:
	std::size_t toCategoryIndex(ETypeCategory category) const;

	static void reportResourceNotFound(const std::string& categoryName, const std::string& name, const DataTreatment& treatment);
};

// template implementations:

template<typename T>
std::shared_ptr<T> NamedResourceStorage::getResource(
	const std::string& resourceName, 
	const DataTreatment& treatment) const
{
	const SdlTypeInfo& typeInfo = T::ciTypeInfo();
	const std::shared_ptr<ISdlResource>& rawResource = getResource(typeInfo, resourceName, treatment);
	if(rawResource == nullptr)
	{
		return nullptr;
	}

	const std::shared_ptr<T> castedResource = std::dynamic_pointer_cast<T>(rawResource);
	if(castedResource == nullptr)
	{
		reportResourceNotFound(SdlTypeInfo::categoryToName(typeInfo.typeCategory), resourceName, treatment);
		std::cerr << "warning: at NamedResourceStorage::getResource(), " 
		          << "resource not found detail: requested type mismatch" << std::endl;
		return nullptr;
	}

	return castedResource;
}

template<typename T>
bool NamedResourceStorage::hasResource(const std::string& resourceName) const
{
	const SdlTypeInfo& typeInfo         = T::ciTypeInfo();
	const std::size_t  categoryIndex    = toCategoryIndex(typeInfo.typeCategory);
	const auto&        resourcesNameMap = m_resources[categoryIndex];
	const auto&        iter             = resourcesNameMap.find(resourceName);

	return iter != resourcesNameMap.end();
}

}// end namespace ph