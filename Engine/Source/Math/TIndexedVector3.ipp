#pragma once

#include "Math/TIndexedVector3.h"
#include "Math/math.h"

#include <iostream>

namespace ph
{

template<typename T>
inline TIndexedVector3<T>::TIndexedVector3() : 
	TVector3<T>(), 
	m_entries{&x, &y, &z}
{

}

template<typename T>
inline TIndexedVector3<T>::TIndexedVector3(const TIndexedVector3& other) : 
	TVector3<T>(other),
	m_entries{&x, &y, &z}
{

}

template<typename T>
inline TIndexedVector3<T>::TIndexedVector3(const TVector3<T>& vector3) : 
	TVector3<T>(vector3),
	m_entries{&x, &y, &z}
{

}

template<typename T>
inline TIndexedVector3<T>::TIndexedVector3(const T x, const T y, const T z) : 
	TVector3<T>(x, y, z),
	m_entries{&(this->x), &(this->y), &(this->z)}
{

}

template<typename T>
inline TIndexedVector3<T>::~TIndexedVector3() = default;

template<typename T>
inline T& TIndexedVector3<T>::operator [] (const int32 axisIndex)
{
	return *(m_entries[axisIndex]);
}

template<typename T>
inline const T& TIndexedVector3<T>::operator [] (const int32 axisIndex) const
{
	return *(m_entries[axisIndex]);
}

template<typename T>
inline TIndexedVector3<T>& TIndexedVector3<T>::operator = (const TIndexedVector3& rhs)
{
	TVector3<T>::operator = (rhs);
	return *this;
}

}// end namespace ph