#pragma once

#include "Math/General/TVectorN.h"

#include <array>

namespace ph::math
{

template<typename T, std::size_t N>
template<typename U>
inline TVectorN<T, N>::TVectorN(const TVectorN<U, N>& other)
{
	for(std::size_t i = 0; i < N; ++i)
	{
		m[i] = static_cast<T>(other[i]);
	}
}

}// end namespace ph::math
