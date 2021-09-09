#pragma once

#include "Math/Color/TSpectrumBase.h"
#include "Math/TArithmeticArray.h"
#include "Common/assertion.h"
#include "Math/math.h"

#include <cmath>

namespace ph::math
{

template<typename Derived, typename T, std::size_t N>
template<typename U>
inline TSpectrumBase<Derived, T, N>::TSpectrumBase(const std::array<U, N>& values)
{
	for(std::size_t i = 0; i < N; ++i)
	{
		m[i] = static_cast<T>(values[i]);
	}
}

template<typename Derived, typename T, std::size_t N>
template<typename U>
inline TSpectrumBase<Derived, T, N>::TSpectrumBase(const TArithmeticArray<U, N>& values)
{
	for(std::size_t i = 0; i < N; ++i)
	{
		m[i] = static_cast<T>(values[i]);
	}
}

template<typename Derived, typename T, std::size_t N>
template<typename U>
inline TSpectrumBase<Derived, T, N>::TSpectrumBase(const U* const values)
{
	PH_ASSERT(values);

	for(std::size_t i = 0; i < N; ++i)
	{
		m[i] = static_cast<T>(values[i]);
	}
}

template<typename Derived, typename T, std::size_t N>
template<typename U>
inline TSpectrumBase<Derived, T, N>::TSpectrumBase(const std::vector<U>& values)
{
	PH_ASSERT_EQ(values.size(), N);

	for(std::size_t i = 0; i < N; ++i)
	{
		m[i] = static_cast<T>(values[i]);
	}
}

template<typename Derived, typename T, std::size_t N>
inline std::size_t TSpectrumBase<Derived, T, N>::minComponent() const
{
	// minIndex() is not exposed; use "this" to access it in current scope
	return this->minIndex();
}

template<typename Derived, typename T, std::size_t N>
inline std::size_t TSpectrumBase<Derived, T, N>::maxComponent() const
{
	// maxIndex() is not exposed; use "this" to access it in current scope
	return this->maxIndex();
}

}// end namespace ph::math
