#pragma once

#include "SDL/sdl_fwd.h"

#include <Common/assertion.h>

#include <string_view>
#include <type_traits>

namespace ph
{

/*! @brief A helper for accessing SDL enum properties.

This is a base type without any functionality implemented. Specializations
are required to provide implementation for the methods, and this is generally done by providing
an enum definition via `PH_DEFINE_SDL_ENUM()`.
*/
template<typename EnumType>
class TSdlEnum final
{
	static_assert(std::is_enum_v<EnumType>,
		"EnumType must be an enum. Currently it is not.");

public:
	/*! @brief Get enum value from its name.
	*/
	EnumType operator [] (const std::string_view entryName) const;

	/*! @brief Get enum name from its value.

	The memory for the view object is backed by the class instance.
	*/
	std::string_view operator [] (EnumType enumValue) const;
};

// In-header Implementations:

template<typename EnumType>
inline EnumType TSdlEnum<EnumType>::operator [] (const std::string_view entryName) const
{
	// Please make sure you provide a specialization of `TSdlEnum` for your enum type.
	PH_ASSERT_UNREACHABLE_SECTION();

	return static_cast<EnumType>(0);
}

template<typename EnumType>
inline std::string_view TSdlEnum<EnumType>::operator [] (const EnumType enumValue) const
{
	// Please make sure you provide a specialization of `TSdlEnum` for your enum type.
	PH_ASSERT_UNREACHABLE_SECTION();

	return "";
}

}// end namespace ph
