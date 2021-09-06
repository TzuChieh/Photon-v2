#pragma once

#include <concepts>

namespace ph::math
{

template<typename T>
concept CColorSpaceConverter = requires ()
{
	{ T::isTristimulus() } noexcept -> std::same_as<bool>
}

}// end namespace ph::math
