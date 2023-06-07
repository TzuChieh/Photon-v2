#pragma once

#include "SDL/ISdlReferenceGroup.h"
#include "SDL/sdl_helpers.h"
#include "SDL/sdl_exceptions.h"
#include "Common/assertion.h"
#include "Utility/utility.h"
#include "Utility/string_utils.h"

#include <unordered_map>
#include <string>
#include <vector>
#include <array>
#include <type_traits>
#include <cstddef>

namespace ph
{

class RawResourceCollection : public ISdlReferenceGroup
{
public:
	RawResourceCollection();

	std::shared_ptr<ISdlResource> get(
		std::string_view resourceName,
		ESdlTypeCategory category) const override;

	bool has(
		std::string_view resourceName,
		ESdlTypeCategory category) const override;

	/*! @brief Add a resource.
	@param resource The resource to be added.
	@param resourceName The name given to the resource.
	*/
	void add(
		std::shared_ptr<ISdlResource> resource,
		std::string_view resourceName);

	/*! @brief Get all resources of type @p T.
	*/
	template<typename T>
	std::vector<std::shared_ptr<T>> getAll() const;

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
	void listAll(
		std::vector<const ISdlResource*>* out_resources,
		std::vector<std::string_view>* out_resourceNames = nullptr) const;

private:
	using ResourceMapType = string_utils::TStdUnorderedStringMap<std::shared_ptr<ISdlResource>>;

	std::array<ResourceMapType, enum_size<ESdlTypeCategory>()> m_resources;
	
private:
	std::size_t toCategoryIndex(ESdlTypeCategory category) const;
	auto getNameToResourceMap(ESdlTypeCategory category) -> ResourceMapType&;
	auto getNameToResourceMap(ESdlTypeCategory category) const -> const ResourceMapType&;
};

inline bool RawResourceCollection::has(
	std::string_view resourceName,
	const ESdlTypeCategory category) const
{
	// Mostly the same as `get(2)`, while saving a copy of shared pointer at the end.

	const auto& nameToResourceMap = getNameToResourceMap(category);

	const auto& iter = nameToResourceMap.find(resourceName);
	return iter != nameToResourceMap.end();
}

template<typename T>
inline std::vector<std::shared_ptr<T>> RawResourceCollection::getAll() const
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

inline std::size_t RawResourceCollection::toCategoryIndex(const ESdlTypeCategory category) const
{
	const std::size_t index = static_cast<std::size_t>(category);
	PH_ASSERT_LT(index, m_resources.size());

	return index;
}

inline auto RawResourceCollection::getNameToResourceMap(const ESdlTypeCategory category)
	-> ResourceMapType&
{
	return m_resources[toCategoryIndex(category)];
}

inline auto RawResourceCollection::getNameToResourceMap(const ESdlTypeCategory category) const
	-> const ResourceMapType&
{
	return m_resources[toCategoryIndex(category)];
}

}// end namespace ph
