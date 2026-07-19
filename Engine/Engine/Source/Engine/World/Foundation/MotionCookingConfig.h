#pragma once

#include <cstddef>
#include <functional>
#include <type_traits>

namespace ph
{

class MotionCookingConfig final
{
public:
	friend bool operator == (const MotionCookingConfig& lhs, const MotionCookingConfig& rhs) = default;
};

// Should be trivially copyable so there are no surprises when being hashed
static_assert(std::is_trivially_copyable_v<MotionCookingConfig>);

}// end namespace ph

namespace std
{

template<>
struct hash<ph::MotionCookingConfig>
{
	std::size_t operator () (const ph::MotionCookingConfig& config) const
	{
		return 0;
	}
};

}// end namespace std
