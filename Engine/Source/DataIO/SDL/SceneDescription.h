#pragma once

#include "Actor/AModel.h"
#include "Actor/ALight.h"
#include "DataIO/SDL/DataTreatment.h"
#include "DataIO/SDL/SdlTypeInfo.h"
#include "DataIO/SDL/ISdlResource.h"

#include <unordered_map>
#include <string>
#include <memory>
#include <vector>
#include <iostream>
#include <array>

namespace ph
{

class SceneDescription final
{
public:
	SceneDescription();

	void addResource(
		const SdlTypeInfo& typeInfo,
		const std::string& resourceName,
		std::unique_ptr<ISdlResource> resource);

	/*! @brief Given type info @p typeInfo, get a resource named @p resourceName.
	*/
	std::shared_ptr<ISdlResource> getResource(
		const SdlTypeInfo& typeInfo,
		const std::string& resourceName,
		const DataTreatment& treatment = DataTreatment()) const;

	/*! @brief Get a resource of type @p T with name @p resourceName.
	*/
	template<typename T>
	std::shared_ptr<T> getResource(
		const std::string& resourceName, 
		const DataTreatment& treatment = DataTreatment()) const;

	/*! @brief Get all resources of type @p T.
	*/
	template<typename T>
	std::vector<std::shared_ptr<T>> getResources() const;

	template<typename T>
	bool hasResource(const std::string& resourceName) const;

	std::vector<std::shared_ptr<Actor>> getActors() const;

private:
	std::array<
		std::unordered_map<std::string, std::shared_ptr<ISdlResource>>, 
		static_cast<std::size_t>(ETypeCategory::MAX) + 1
	> m_resources;
	
private:
	std::size_t toCategoryIndex(ETypeCategory category) const;

	static void reportResourceNotFound(const std::string& categoryName, const std::string& name, const DataTreatment& treatment);
};

// In-header Implementations:

template<typename T>
inline std::shared_ptr<T> SceneDescription::getResource(
	const std::string& resourceName, 
	const DataTreatment& treatment) const
{
	// TODO: check T::ciTypeInfo() exists

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
		std::cerr << "warning: at SceneDescription::getResource(), " 
		          << "resource not found detail: requested type mismatch" << std::endl;
		return nullptr;
	}

	return castedResource;
}

template<typename T>
inline std::vector<std::shared_ptr<T>> SceneDescription::getResources() const
{
	// TODO: check T::ciTypeInfo() exists

	const SdlTypeInfo& typeInfo         = T::ciTypeInfo();
	const std::size_t  categoryIndex    = toCategoryIndex(typeInfo.typeCategory);
	const auto&        resourcesNameMap = m_resources[categoryIndex];

	std::vector<std::shared_ptr<T>> resources;
	for(const auto& [name, resource] : resourcesNameMap)
	{
		const std::shared_ptr<T> castedResource = std::dynamic_pointer_cast<T>(resource);
		if(castedResource)
		{
			resources.push_back(std::move(castedResource));
		}
	}

	return resources;
}

template<typename T>
inline bool SceneDescription::hasResource(const std::string& resourceName) const
{
	const SdlTypeInfo& typeInfo         = T::ciTypeInfo();
	const std::size_t  categoryIndex    = toCategoryIndex(typeInfo.typeCategory);
	const auto&        resourcesNameMap = m_resources[categoryIndex];
	const auto&        iter             = resourcesNameMap.find(resourceName);

	return iter != resourcesNameMap.end();
}

}// end namespace ph
