#pragma once

#include "SDL/ISdlResource.h"
#include "SDL/ESdlTypeCategory.h"
#include "SDL/sdl_helpers.h"
#include "SDL/sdl_exceptions.h"

#include <string_view>
#include <memory>
#include <type_traits>

namespace ph
{

class ISdlReferenceGroup
{
public:
	inline ISdlReferenceGroup() = default;
	inline ISdlReferenceGroup(const ISdlReferenceGroup& other) = default;
	inline ISdlReferenceGroup(ISdlReferenceGroup&& other) = default;
	inline virtual ~ISdlReferenceGroup() = default;

	/*! @brief Get a resource reference.
	@param resourceName The name of the resource.
	@param category Category of the resource.
	@return The resource requested. `nullptr` if not found.
	*/
	virtual std::shared_ptr<ISdlResource> get(
		std::string_view resourceName,
		ESdlTypeCategory category) const = 0;

	/*! @brief Check the existence of a resource reference.
	@param resourceName The name of the resource.
	@param category Category of the resource.
	*/
	virtual bool has(
		std::string_view resourceName, 
		ESdlTypeCategory category) const = 0;
	
	/*! @brief Get a resource reference of type @p T with name @p resourceName.
	@return The resource requested. `nullptr` if not found.
	@exception SdlLoadError If the requested resource is not of type @p T.
	*/
	template<typename T>
	std::shared_ptr<T> getTyped(std::string_view resourceName) const;

	/*! @brief Check the existence of resource of type @p T.
	*/
	template<typename T>
	bool hasTyped(std::string_view resourceName) const;

	inline ISdlReferenceGroup& operator = (const ISdlReferenceGroup& rhs) = default;
	inline ISdlReferenceGroup& operator = (ISdlReferenceGroup&& rhs) = default;
};

template<typename T>
inline std::shared_ptr<T> ISdlReferenceGroup::getTyped(std::string_view resourceName) const
{
	static_assert(std::is_base_of_v<ISdlResource, T>,
		"T is not a SDL resource.");

	const ESdlTypeCategory category = sdl::category_of<T>();
	std::shared_ptr<ISdlResource> rawResource = get(resourceName, category);
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

		throw_formatted<SdlLoadError>(
			"expected resource type different from the requested type (category: {}, name: {})",
			sdl::category_to_string(category), resourceName);
	}

	return castedResource;
}

template<typename T>
inline bool ISdlReferenceGroup::hasTyped(std::string_view resourceName) const
{
	static_assert(std::is_base_of_v<ISdlResource, T>,
		"T is not a SDL resource.");

	return has(resourceName, sdl::category_of<T>());
}

}// end namespace ph
