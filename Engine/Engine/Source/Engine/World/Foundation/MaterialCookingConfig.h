#pragma once

#include <cstddef>
#include <functional>
#include <type_traits>

namespace ph
{

class MaterialCookingConfig final
{
public:
	friend bool operator == (const MaterialCookingConfig& lhs, const MaterialCookingConfig& rhs) = default;
};

// Should be trivially copyable so there are no surprises when being hashed
static_assert(std::is_trivially_copyable_v<MaterialCookingConfig>);

}// end namespace ph

namespace std
{

template<>
struct hash<ph::MaterialCookingConfig>
{
	std::size_t operator () (const ph::MaterialCookingConfig& config) const
	{
		return 0;
	}
};

}// end namespace std
