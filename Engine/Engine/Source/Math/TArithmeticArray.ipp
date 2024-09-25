#pragma once

#include "Math/TArithmeticArray.h"

namespace ph::math
{

template<typename T, std::size_t N>
template<typename U>
inline TArithmeticArray<T, N>::TArithmeticArray(const TArithmeticArray<U, N>& other)
{
	for(std::size_t i = 0; i < N; ++i)
	{
		m[i] = static_cast<T>(other[i]);
	}
}

#if PH_COMPILER_HAS_P2468R2

template<typename T, std::size_t N>
inline bool TArithmeticArray<T, N>::operator == (const Self& other) const
{
	return Base::operator == (other);
}

#endif


}// end namespace ph::math
