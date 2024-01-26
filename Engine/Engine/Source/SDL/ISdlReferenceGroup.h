#pragma once

#include "SDL/ISdlResource.h"
#include "SDL/sdl_helpers.h"
#include "SDL/sdl_exceptions.h"
#include "Utility/utility.h"

#include <string_view>
#include <memory>
#include <type_traits>

namespace ph
{

/*! @brief View for a group of SDL references.
*/
class ISdlReferenceGroup
{
public:
	PH_DEFINE_INLINE_RULE_OF_5_MEMBERS_NO_DTOR(ISdlReferenceGroup);
	virtual ~ISdlReferenceGroup() = default;

	/*! @brief Get a resource reference.
	@param resourceName The name of the resource.
	@return The resource requested. `nullptr` if not found.
	*/
	virtual std::shared_ptr<ISdlResource> get(std::string_view resourceName) const = 0;

	/*! @brief Check the existence of a resource reference.
	@param resourceName The name of the resource.
	*/
	virtual bool has(std::string_view resourceName) const = 0;
	
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
};

template<typename T>
inline std::shared_ptr<T> ISdlReferenceGroup::getTyped(std::string_view resourceName) const
{
	static_assert(std::is_base_of_v<ISdlResource, T>,
		"T is not a SDL resource.");

	std::shared_ptr<ISdlResource> rawResource = get(resourceName);
	if(!rawResource)
	{
		return nullptr;
	}

	std::shared_ptr<T> castedResource = std::dynamic_pointer_cast<T>(std::move(rawResource));
	if(!castedResource)
	{
		// The cast can fail if a wrong type for the resource is specified
		// (name is correct, but with a wrong type).

		throw_formatted<SdlLoadError>(
			"resource type (category: {}) is not the requested type (category: {}, name: {})",
			sdl::category_to_string(get(resourceName)->getDynamicCategory()),
			sdl::category_to_string(sdl::category_of<T>()), resourceName);
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
