#pragma once

#include "Math/TQuaternion.h"
#include "Math/TVector3.h"
#include "Math/TMatrix4.h"

namespace ph
{

template<typename T>
inline TQuaternion<T>::TQuaternion() :
	x(0), y(0), z(0), w(1)
{

}

template<typename T>
inline TQuaternion<T>::TQuaternion(const TQuaternion& other) :
	x(other.x), y(other.y), z(other.z), w(other.w)
{

}

template<typename T>
template<typename U>
inline TQuaternion<T>::TQuaternion(const TQuaternion<U>& other) : 
	x(static_cast<T>(other.x)), y(static_cast<T>(other.y)), z(static_cast<T>(other.z)), w(static_cast<T>(other.w))
{

}

template<typename T>
inline TQuaternion<T>::TQuaternion(const T x, const T y, const T z, const T w) :
	x(x), y(y), z(z), w(w)
{

}

template<typename T>
inline TQuaternion<T>::TQuaternion(const TVector3<T>& normalizedAxis, const T radians) :
	TQuaternion()
{
	setRot(normalizedAxis, radians);
}

// acting like w component is 0
template<typename T>
inline TQuaternion<T> TQuaternion<T>::mul(const TVector3<T>& xyz) const
{
	return TQuaternion(w * xyz.x + y * xyz.z - z * xyz.y,
	                   w * xyz.y - x * xyz.z + z * xyz.x,
	                   w * xyz.z + x * xyz.y - y * xyz.x,
	                  -x * xyz.x - y * xyz.y - z * xyz.z);
}

template<typename T>
inline void TQuaternion<T>::mul(const TVector3<T>& xyz, TQuaternion* const out_result) const
{
	out_result->x =  w * xyz.x + y * xyz.z - z * xyz.y;
	out_result->y =  w * xyz.y - x * xyz.z + z * xyz.x;
	out_result->z =  w * xyz.z + x * xyz.y - y * xyz.x;
	out_result->w = -x * xyz.x - y * xyz.y - z * xyz.z;
}

template<typename T>
inline void TQuaternion<T>::setRot(const TVector3<T>& normalizedAxis, const T radians)
{
	const T sinHalfAngle = std::sin(radians / 2);
	const T cosHalfAngle = std::cos(radians / 2);

	x = normalizedAxis.x * sinHalfAngle;
	y = normalizedAxis.y * sinHalfAngle;
	z = normalizedAxis.z * sinHalfAngle;
	w = cosHalfAngle;
}

template<typename T>
inline void TQuaternion<T>::toRotationMatrix(TMatrix4<T>* const out_result) const
{
	out_result->m[0][0] = 1 - 2*(y*y + z*z);
	out_result->m[0][1] = 2*(x*y - z*w);
	out_result->m[0][2] = 2*(y*w + x*z);
	out_result->m[0][3] = 0;

	out_result->m[1][0] = 2*(x*y + z*w);
	out_result->m[1][1] = 1 - 2*(x*x + z*z);
	out_result->m[1][2] = 2*(y*z - x*w);
	out_result->m[1][3] = 0;

	out_result->m[2][0] = 2*(x*z - y*w);
	out_result->m[2][1] = 2*(y*z + x*w);
	out_result->m[2][2] = 1 - 2*(x*x + y*y);
	out_result->m[2][3] = 0;

	out_result->m[3][0] = 0;
	out_result->m[3][1] = 0;
	out_result->m[3][2] = 0;
	out_result->m[3][3] = 1;
}

template<typename T>
inline TQuaternion<T> TQuaternion<T>::normalize() const
{
	const T reciLen = 1 / length();
	return TQuaternion(x * reciLen, y * reciLen, z * reciLen, w * reciLen);
}

template<typename T>
inline TQuaternion<T>& TQuaternion<T>::normalizeLocal()
{
	const T reciLen = 1 / length();

	x *= reciLen;
	y *= reciLen;
	z *= reciLen;
	w *= reciLen;

	return *this;
}

template<typename T>
inline T TQuaternion<T>::length() const
{
	return std::sqrt(x * x + y * y + z * z + w * w);
}

template<typename T>
inline TQuaternion<T> TQuaternion<T>::conjugate() const
{
	return TQuaternion(-x, -y, -z, w);
}

template<typename T>
inline void TQuaternion<T>::conjugate(TQuaternion* const out_result) const
{
	out_result->x = -x;
	out_result->y = -y;
	out_result->z = -z;
	out_result->w = w;
}

template<typename T>
inline TQuaternion<T>& TQuaternion<T>::conjugateLocal()
{
	x *= -1;
	y *= -1;
	z *= -1;

	return *this;
}

template<typename T>
inline TQuaternion<T> TQuaternion<T>::mul(const TQuaternion& rhs) const
{
	const T _x = w * rhs.x + x * rhs.w + y * rhs.z - z * rhs.y;
	const T _y = w * rhs.y - x * rhs.z + y * rhs.w + z * rhs.x;
	const T _z = w * rhs.z + x * rhs.y - y * rhs.x + z * rhs.w;
	const T _w = w * rhs.w - x * rhs.x - y * rhs.y - z * rhs.z;

	return TQuaternion(_x, _y, _z, _w);
}

template<typename T>
inline void TQuaternion<T>::mul(const TQuaternion& rhs, TQuaternion* const out_result) const
{
	out_result->x = w * rhs.x + x * rhs.w + y * rhs.z - z * rhs.y;
	out_result->y = w * rhs.y - x * rhs.z + y * rhs.w + z * rhs.x;
	out_result->z = w * rhs.z + x * rhs.y - y * rhs.x + z * rhs.w;
	out_result->w = w * rhs.w - x * rhs.x - y * rhs.y - z * rhs.z;
}

template<typename T>
inline TQuaternion<T>& TQuaternion<T>::mulLocal(const TQuaternion& rhs)
{
	const T _x = w * rhs.x + x * rhs.w + y * rhs.z - z * rhs.y;
	const T _y = w * rhs.y - x * rhs.z + y * rhs.w + z * rhs.x;
	const T _z = w * rhs.z + x * rhs.y - y * rhs.x + z * rhs.w;
	const T _w = w * rhs.w - x * rhs.x - y * rhs.y - z * rhs.z;

	x = _x;
	y = _y;
	z = _z;
	w = _w;

	return *this;
}

template<typename T>
inline TQuaternion<T> TQuaternion<T>::mul(const T rhs) const
{
	return TQuaternion(x * rhs, y * rhs, z * rhs, w * rhs);
}

template<typename T>
inline TQuaternion<T> TQuaternion<T>::sub(const TQuaternion& rhs) const
{
	return TQuaternion(x - rhs.x, y - rhs.y, z - rhs.z, w - rhs.w);
}

template<typename T>
inline TQuaternion<T> TQuaternion<T>::add(const TQuaternion& rhs) const
{
	return TQuaternion(x + rhs.x, y + rhs.y, z + rhs.z, w + rhs.w);
}

template<typename T>
inline T TQuaternion<T>::dot(const TQuaternion& rhs) const
{
	return x * rhs.x + y * rhs.y + z * rhs.z + w * rhs.w;
}

template<typename T>
inline TQuaternion<T>& TQuaternion<T>::set(const T rhsX, const T rhsY, const T rhsZ, const T rhsW)
{
	x = rhsX;
	y = rhsY;
	z = rhsZ;
	w = rhsW;

	return *this;
}

template<typename T>
inline TQuaternion<T>& TQuaternion<T>::set(const TQuaternion& rhs)
{
	x = rhs.x;
	y = rhs.y;
	z = rhs.z;
	w = rhs.w;

	return *this;
}

template<typename T>
inline std::string TQuaternion<T>::toStringFormal() const
{
	return "(" + std::to_string(x) + ", " 
	           + std::to_string(y) + ", " 
	           + std::to_string(z) + ", " 
	           + std::to_string(w) + ")";
}

}// end namespace ph