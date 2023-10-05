#pragma once

#include "SDL/ISdlReferenceGroup.h"
#include "SDL/sdl_exceptions.h"
#include "Common/assertion.h"
#include "Utility/utility.h"
#include "Utility/string_utils.h"

#include <unordered_map>
#include <string>
#include <vector>
#include <type_traits>
#include <cstddef>
#include <utility>

namespace ph
{

class RawResourceCollection : public ISdlReferenceGroup
{
public:
	RawResourceCollection();

	std::shared_ptr<ISdlResource> get(std::string_view resourceName) const override;
	bool has(std::string_view resourceName) const override;

	/*! @brief Add a resource.
	@param resource The resource to be added.
	@param resourceName The name given to the resource.
	@exception SdlLoadError If input data is incomplete, e.g., being empty/null.
	*/
	void add(
		std::shared_ptr<ISdlResource> resource,
		std::string_view resourceName);

	/*! @brief Remove a resource.
	@param resourceName The name given to the resource.
	@return The removed resource. Null if no resource named `resourceName`.
	*/
	std::shared_ptr<ISdlResource> remove(std::string_view resourceName);

	/*! @brief Rename a resource.
	The new name must be unique within this collection. If `newResourceName` is not a unique name,
	a unique name is automatically determined (typically via `makeResourceName()`).
	@param resourceName Name of the resource to be renamed.
	@param newResourceName The intended new resource name.
	@return The new resource name that the resource as renamed into.
	@exception SdlLoadError If there is no resource named `resourceName`.
	*/
	std::string rename(std::string_view resourceName, std::string_view newResourceName);

	/*! @brief Create a resource name that is unique within this collection.
	@param intendedName If the name given is already unique, then it will be used; otherwise, the
	name will be changed such that the resulting name is unique.
	*/
	std::string makeResourceName(std::string_view intendedName);

	/*! @brief Get all resources of type @p T.
	*/
	template<typename T>
	std::vector<std::shared_ptr<T>> getAllOfType() const;

	/*! @brief Get a list of all resources of type @p T.
	*/
	template<typename T>
	std::vector<T*> listAllOfType() const;

	/*! @brief Get a list of all resources in the scene.
	Does not involve any change in ownership. Useful for operations that simply want to iterate
	over all resources.
	@param out_resourceNames Storage for all resource names (1-to-1 mapping to @p out_resources).
	Left unspecified (nullptr) if resource names are not required.
	@return All resources.
	@note Changing the state of the scene may invalidate the returned resource pointers as well as
	the returned names.
	*/
	std::vector<const ISdlResource*> listAll(
		std::vector<std::string>* out_resourceNames = nullptr) const;

private:
	using ResourceMapType = string_utils::TStdUnorderedStringMap<std::shared_ptr<ISdlResource>>;

	ResourceMapType m_nameToResource;
};

inline std::shared_ptr<ISdlResource> RawResourceCollection::get(std::string_view resourceName) const
{
	const auto& iter = m_nameToResource.find(resourceName);
	return iter != m_nameToResource.end() ? iter->second : nullptr;
}

inline bool RawResourceCollection::has(std::string_view resourceName) const
{
	// Mostly the same as `get(1)`, while saving a copy of shared pointer.

	const auto& iter = m_nameToResource.find(resourceName);
	return iter != m_nameToResource.end();
}

template<typename T>
inline std::vector<std::shared_ptr<T>> RawResourceCollection::getAllOfType() const
{
	static_assert(std::is_base_of_v<ISdlResource, T>,
		"T is not a SDL resource.");

	std::vector<std::shared_ptr<T>> resources;
	for(const auto& [name, resource] : m_nameToResource)
	{
		std::shared_ptr<T> castedResource = std::dynamic_pointer_cast<T>(resource);
		if(castedResource)
		{
			resources.push_back(std::move(castedResource));
		}
	}
	return resources;
}

template<typename T>
inline std::vector<T*> RawResourceCollection::listAllOfType() const
{
	static_assert(std::is_base_of_v<ISdlResource, T>,
		"T is not a SDL resource.");
	
	std::vector<T*> resources;
	for(const auto& [name, resource] : m_nameToResource)
	{
		T* castedResource = dynamic_cast<T*>(resource.get());
		if(castedResource)
		{
			resources.push_back(castedResource);
		}
	}
	return resources;
}

}// end namespace ph
