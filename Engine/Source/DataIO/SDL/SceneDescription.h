#pragma once

#include "DataIO/SDL/ISdlResource.h"
#include "DataIO/SDL/ETypeCategory.h"
#include "DataIO/SDL/sdl_helpers.h"
#include "DataIO/SDL/sdl_exceptions.h"
#include "Common/assertion.h"

#include <unordered_map>
#include <string>
#include <string_view>
#include <memory>
#include <vector>
#include <array>
#include <type_traits>
#include <cstddef>

namespace ph
{

class SceneDescription final
{
public:
	SceneDescription();

	/*! @brief Add a resource.

	@param resource The resource to be added.
	@param resourceName The name given to the resource.
	*/
	void addResource(
		std::shared_ptr<ISdlResource> resource,
		const std::string&            resourceName);

	/*! @brief Get a resource.

	@param resourceName The name of the resource.
	@param category Category of the resource.
	@return The resource requested. `nullptr` if not found.
	*/
	std::shared_ptr<ISdlResource> getResource(
		const std::string& resourceName,
		ETypeCategory      category) const;
	
	/*! @brief Get a resource of type @p T with name @p resourceName.

	@return The resource requested. `nullptr` if not found.
	@exception SdlLoadError If the requested resource is not of type @p T.
	*/
	template<typename T>
	std::shared_ptr<T> getResource(
		const std::string& resourceName) const;

	// TODO: allow type mismatch?


	/*! @brief Get all resources of type @p T.
	*/
	template<typename T>
	std::vector<std::shared_ptr<T>> getResources() const;

	/*! @brief Check the existence of resource of type @p T.
	*/
	template<typename T>
	bool hasResource(const std::string& resourceName) const;

	/*! @brief Get a list of all resources in the scene.
	
	Does not involve any change in ownership. Useful for operations that simply needs
	to iterate over all resources.

	@param out_resources Storage for all resources.
	@param out_resourceNames Storage for all resource names (1-to-1 mapping to @p out_resources).
	The memory of resource names are backed by this SceneDescription. Left unspecified (nullptr) 
	if resource names are not required.

	@note Changing the state of the scene may invalidate the returned resource pointers as well as
	the returned names.
	*/
	void listAllResources(
		std::vector<const ISdlResource*>* out_resources,
		std::vector<std::string_view>*    out_resourceNames = nullptr) const;

private:
	std::array<
		std::unordered_map<std::string, std::shared_ptr<ISdlResource>>, 
		static_cast<std::size_t>(ETypeCategory::NUM)
	> m_resources;
	
private:
	std::size_t toCategoryIndex(ETypeCategory category) const;
	auto getNameToResourceMap(ETypeCategory category) -> std::unordered_map<std::string, std::shared_ptr<ISdlResource>>&;
	auto getNameToResourceMap(ETypeCategory category) const -> const std::unordered_map<std::string, std::shared_ptr<ISdlResource>>&;
};

// In-header Implementations:

template<typename T>
inline std::shared_ptr<T> SceneDescription::getResource(
	const std::string& resourceName) const
{
	static_assert(std::is_base_of_v<ISdlResource, T>,
		"T is not a SDL resource.");

	const ETypeCategory category = sdl::category_of<T>();
	std::shared_ptr<ISdlResource> rawResource = getResource(resourceName, category);
	if(!rawResource)
	{
		return nullptr;
	}

	std::shared_ptr<T> castedResource = std::dynamic_pointer_cast<T>(std::move(rawResource));
	if(!castedResource)
	{
		// Though the category cannot be wrong as the information is from the
		// type itself, the cast can still fail if a wrong type for the resource
		// is specified (within the same category, but a wrong type).

		throw SdlLoadError(
			"expected resource type different from the requested type "
			"(category: " + sdl::category_to_string(category) + ", name: " + resourceName + ")");
	}

	return std::move(castedResource);
}

template<typename T>
inline std::vector<std::shared_ptr<T>> SceneDescription::getResources() const
{
	static_assert(std::is_base_of_v<ISdlResource, T>,
		"T is not a SDL resource.");

	const auto& nameToResourceMap = getNameToResourceMap(sdl::category_of<T>());

	std::vector<std::shared_ptr<T>> resources;
	for(const auto& [name, resource] : nameToResourceMap)
	{
		std::shared_ptr<T> castedResource = std::dynamic_pointer_cast<T>(resource);
		if(castedResource)
		{
			resources.push_back(std::move(castedResource));
		}
	}

	return std::move(resources);
}

template<typename T>
inline bool SceneDescription::hasResource(const std::string& resourceName) const
{
	static_assert(std::is_base_of_v<ISdlResource, T>,
		"T is not a SDL resource.");

	const auto& nameToResourceMap = getNameToResourceMap(sdl::category_of<T>());

	const auto& iter = nameToResourceMap.find(resourceName);
	return iter != nameToResourceMap.end();
}

inline std::size_t SceneDescription::toCategoryIndex(const ETypeCategory category) const
{
	const std::size_t index = static_cast<std::size_t>(category);
	PH_ASSERT_LT(index, m_resources.size());

	return index;
}

inline auto SceneDescription::getNameToResourceMap(const ETypeCategory category)
	-> std::unordered_map<std::string, std::shared_ptr<ISdlResource>>&
{
	return m_resources[toCategoryIndex(category)];
}

inline auto SceneDescription::getNameToResourceMap(const ETypeCategory category) const
	-> const std::unordered_map<std::string, std::shared_ptr<ISdlResource>>&
{
	return m_resources[toCategoryIndex(category)];
}

}// end namespace ph
