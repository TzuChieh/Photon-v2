#pragma once

#include "Math/TVector4.h"
#include "Math/math.h"

#include <Common/assertion.h>

#include <cmath>
#include <algorithm>
#include <type_traits>
#include <array>

namespace ph::math
{

template<typename T>
inline TVector4<T>::TVector4(const T vx, const T vy, const T vz, const T vw) :
	Base(std::array<T, 4>{vx, vy, vz, vw})
{}

template<typename T>
template<typename U>
inline TVector4<T>::TVector4(const TVector4<U>& other) :
	TVector4(
		static_cast<T>(other[0]), 
		static_cast<T>(other[1]),
		static_cast<T>(other[2]),
		static_cast<T>(other[3]))
{}

template<typename T>
inline T& TVector4<T>::x()
{
	return m[0];
}

template<typename T>
inline T& TVector4<T>::y()
{
	return m[1];
}

template<typename T>
inline T& TVector4<T>::z()
{
	return m[2];
}

template<typename T>
inline T& TVector4<T>::w()
{
	return m[3];
}

template<typename T>
inline const T& TVector4<T>::x() const
{
	return m[0];
}

template<typename T>
inline const T& TVector4<T>::y() const
{
	return m[1];
}

template<typename T>
inline const T& TVector4<T>::z() const
{
	return m[2];
}

template<typename T>
inline const T& TVector4<T>::w() const
{
	return m[3];
}

template<typename T>
inline T& TVector4<T>::r()
{
	return m[0];
}

template<typename T>
inline T& TVector4<T>::g()
{
	return m[1];
}

template<typename T>
inline T& TVector4<T>::b()
{
	return m[2];
}

template<typename T>
inline T& TVector4<T>::a()
{
	return m[3];
}

template<typename T>
inline const T& TVector4<T>::r() const
{
	return m[0];
}

template<typename T>
inline const T& TVector4<T>::g() const
{
	return m[1];
}

template<typename T>
inline const T& TVector4<T>::b() const
{
	return m[2];
}

template<typename T>
inline const T& TVector4<T>::a() const
{
	return m[3];
}

}// end namespace ph::math
