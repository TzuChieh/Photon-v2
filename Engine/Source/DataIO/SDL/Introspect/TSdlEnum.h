#pragma once

#include "Common/assertion.h"

#include <string_view>
#include <type_traits>

namespace ph
{

template<typename EnumType>
class TSdlEnum final
{
	static_assert(std::is_enum_v<EnumType>,
		"EnumType must be an enum. Currently it is not.");

public:
	inline EnumType operator [] (const std::string_view entryName) const;
};

// In-header Implementations:

template<typename EnumType>
inline EnumType TSdlEnum::operator [] (const std::string_view entryName) const
{
	PH_ASSERT_UNREACHABLE_SECTION();

	return static_cast<EnumType>(0);
}

}// end namespace ph
