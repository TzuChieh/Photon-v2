#pragma once

#include "Math/TPoint3.h"
#include "Math/TVector3.h"

namespace ph
{

template<typename T>
inline TPoint3<T>::TPoint3() : 
	x(0), y(0), z(0)
{

}

template<typename T>
inline TPoint3<T>::TPoint3(const T x, const T y, const T z) : 
	x(x), y(y), z(z)
{

}

template<typename T>
inline TPoint3<T>::TPoint3(const T value) : 
	x(value), y(value), z(value)
{

}

template<typename T>
inline TPoint3<T>::TPoint3(const TPoint3& other) : 
	x(other.x), y(other.y), z(other.z)
{

}

template<typename T>
template<typename U>
inline TPoint3<T>::TPoint3(const TPoint3<U>& other) : 
	x(static_cast<T>(other.x)), 
	y(static_cast<T>(other.y)), 
	z(static_cast<T>(other.z))
{

}

template<typename T>
template<typename U>
inline TPoint3<T>::TPoint3(const TVector3<U>& vector) :
	x(static_cast<T>(vector.x)),
	y(static_cast<T>(vector.y)),
	z(static_cast<T>(vector.z))
{

}

template<typename T>
inline TPoint3<T>::~TPoint3() = default;

template<typename T>
inline void TPoint3<T>::add(const TVector3<T>& rhs,
                            TPoint3* const out_result) const
{
	out_result->x = x + rhs.x;
	out_result->y = y + rhs.y;
	out_result->z = z + rhs.z;
}

template<typename T>
inline TPoint3<T> TPoint3<T>::add(const TVector3<T>& rhs) const
{
	TPoint3 result;
	this->add(rhs, &result);
	return result;
}

template<typename T>
inline TPoint3<T>& TPoint3<T>::addLocal(const TVector3<T>& rhs)
{
	x += rhs.x;
	y += rhs.y;
	z += rhs.z;

	return *this;
}

template<typename T>
inline void TPoint3<T>::sub(const TPoint3& rhs, 
                            TVector3<T>* const out_result) const
{
	out_result->x = x - rhs.x;
	out_result->y = y - rhs.y;
	out_result->z = z - rhs.z;
}

template<typename T>
inline TVector3<T> TPoint3<T>::sub(const TPoint3& rhs) const
{
	TVector3<T> result;
	this->sub(rhs, &result);
	return result;
}

template<typename T>
inline void TPoint3<T>::sub(const TVector3<T>& rhs, 
                            TPoint3* const out_result) const
{
	out_result->x = x - rhs.x;
	out_result->y = y - rhs.y;
	out_result->z = z - rhs.z;
}

template<typename T>
inline TPoint3<T> TPoint3<T>::sub(const TVector3<T>& rhs) const
{
	TPoint3 result;
	this->sub(rhs, &result);
	return result;
}

template<typename T>
inline TPoint3<T>& TPoint3<T>::subLocal(const TVector3<T>& rhs)
{
	x -= rhs.x;
	y -= rhs.y;
	z -= rhs.z;

	return *this;
}

template<typename T>
inline T TPoint3<T>::distance(const TPoint3& rhs) const
{
	return this->sub(rhs).length();
}

template<typename T>
inline T TPoint3<T>::distanceSquared(const TPoint3& rhs) const
{
	return this->sub(rhs).lengthSquared();
}

}// end namespace ph