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
	*/
	void add(
		std::shared_ptr<ISdlResource> resource,
		std::string_view resourceName);

	/*! @brief Get all resources of type @p T.
	*/
	template<typename T>
	std::vector<std::shared_ptr<T>> getAll() const;

	/*! @brief Get a list of all resources in the scene.
	Does not involve any change in ownership. Useful for operations that simply want to iterate
	over all resources.
	@param out_resources Storage for all resources.
	@param out_resourceNames Storage for all resource names (1-to-1 mapping to @p out_resources).
	Left unspecified (nullptr) if resource names are not required.
	@note Changing the state of the scene may invalidate the returned resource pointers as well as
	the returned names.
	*/
	// TODO: use span
	void listAll(
		std::vector<const ISdlResource*>& out_resources,
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
inline std::vector<std::shared_ptr<T>> RawResourceCollection::getAll() const
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

	return std::move(resources);
}

}// end namespace ph
