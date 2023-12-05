#pragma once

#include "Math/TVector2.h"
#include "Math/math.h"
#include "Utility/utility.h"

#include <Common/assertion.h>

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
	TVector2(static_cast<T>(other[0]), static_cast<T>(other[1]))
{}

template<typename T>
template<typename U>
inline TVector2<U> TVector2<T>::losslessCast() const
{
	return TVector2<U>(
		lossless_cast<U>(m[0]),
		lossless_cast<U>(m[1]));
}

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

template<typename T>
inline T& TVector2<T>::u()
{
	return m[0];
}

template<typename T>
inline T& TVector2<T>::v()
{
	return m[1];
}

template<typename T>
inline const T& TVector2<T>::u() const
{
	return m[0];
}

template<typename T>
inline const T& TVector2<T>::v() const
{
	return m[1];
}

}// end namespace ph::math
