#pragma once

#include "Math/TVector2.h"
#include "Math/math.h"
#include "Common/assertion.h"

#include <cmath>
#include <algorithm>
#include <type_traits>
#include <array>

namespace ph::math
{

template<typename T>
inline TVector2<T>::TVector2(const T vx, const T vy) : 
	Base(std::array<T, 2>{vx, vy})
{}

template<typename T>
template<typename U>
inline TVector2<T>::TVector2(const TVector2<U>& other) : 
	TVector2(static_cast<T>(other.x()), static_cast<T>(other.y()))
{}

template<typename T>
inline T& TVector2<T>::x()
{
	return m[0];
}

template<typename T>
inline T& TVector2<T>::y()
{
	return m[1];
}

template<typename T>
inline const T& TVector2<T>::x() const
{
	return m[0];
}

template<typename T>
inline const T& TVector2<T>::y() const
{
	return m[1];
}

}// end namespace ph::math
