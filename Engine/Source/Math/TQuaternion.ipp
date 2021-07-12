#pragma once

#include "Math/TQuaternion.h"
#include "Math/TVector3.h"
#include "Math/TMatrix4.h"

namespace ph::math
{

template<typename T>
inline TQuaternion<T> TQuaternion<T>::makeNoRotation()
{
	return TQuaternion(0, 0, 0, 1);
}

template<typename T>
inline TQuaternion<T>::TQuaternion(const T x, const T y, const T z, const T w) :
	x(x), y(y), z(z), w(w)
{}

template<typename T>
template<typename U>
inline TQuaternion<T>::TQuaternion(const TQuaternion<U>& other) : 
	TQuaternion(
		static_cast<T>(other.x), 
		static_cast<T>(other.y),
		static_cast<T>(other.z),
		static_cast<T>(other.w))
{}

template<typename T>
template<typename U>
inline TQuaternion<T>::TQuaternion(const std::array<U, 4>& xyzwValues) :
	TQuaternion(TQuaternion<U>(xyzwValues[0], xyzwValues[1], xyzwValues[2], xyzwValues[3]))
{}

template<typename T>
inline TQuaternion<T>::TQuaternion(const TVector3<T>& normalizedAxis, const T radians) :
	TQuaternion()
{
	setRot(normalizedAxis, radians);
}

template<typename T>
inline TQuaternion<T>::TQuaternion(const TMatrix4<T>& rotationMatrix) : 
	TQuaternion()
{
	const TMatrix4<T>& m = rotationMatrix;

	const T trace = m.m[0][0] + m.m[1][1] + m.m[2][2];
	if(trace > 0)
	{
		const T s = static_cast<T>(0.5) / std::sqrt(trace + 1);
		x = (m.m[2][1] - m.m[1][2]) * s;
		y = (m.m[0][2] - m.m[2][0]) * s;
		z = (m.m[1][0] - m.m[0][1]) * s;
		w = static_cast<T>(0.25) / s;
	}
	else
	{
		if(m.m[0][0] > m.m[1][1] && m.m[0][0] > m.m[2][2])
		{
			const T s = static_cast<T>(0.5) / std::sqrt(1 + m.m[0][0] - m.m[1][1] - m.m[2][2]);
			x = static_cast<T>(0.25) / s;
			y = (m.m[0][1] + m.m[1][0]) * s;
			z = (m.m[0][2] + m.m[2][0]) * s;
			w = (m.m[2][1] - m.m[1][2]) * s;
		}
		else if(m.m[1][1] > m.m[2][2])
		{
			const T s = static_cast<T>(0.5) / std::sqrt(1 + m.m[1][1] - m.m[0][0] - m.m[2][2]);
			x = (m.m[0][1] + m.m[1][0]) * s;
			y = static_cast<T>(0.25) / s;
			z = (m.m[1][2] + m.m[2][1]) * s;
			w = (m.m[0][2] - m.m[2][0]) * s;
		}
		else
		{
			const T s = static_cast<T>(0.5) / std::sqrt(1 + m.m[2][2] - m.m[0][0] - m.m[1][1]);
			x = (m.m[0][2] + m.m[2][0]) * s;
			y = (m.m[1][2] + m.m[2][1]) * s;
			z = static_cast<T>(0.25) / s;
			w = (m.m[1][0] - m.m[0][1]) * s;
		}
	}

	normalizeLocal();
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
	const T rcpLen = 1 / length();
	return TQuaternion(x * rcpLen, y * rcpLen, z * rcpLen, w * rcpLen);
}

template<typename T>
inline TQuaternion<T>& TQuaternion<T>::normalizeLocal()
{
	const T rcpLen = 1 / length();

	x *= rcpLen;
	y *= rcpLen;
	z *= rcpLen;
	w *= rcpLen;

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
inline std::string TQuaternion<T>::toString() const
{
	return '(' + std::to_string(x) + ", " 
	           + std::to_string(y) + ", " 
	           + std::to_string(z) + ", " 
	           + std::to_string(w) + ')';
}

template<typename T>
inline bool TQuaternion<T>::isEqual(const TQuaternion& other) const
{
	return x == other.x &&
	       y == other.y &&
	       z == other.z &&
	       w == other.w;
}

template<typename T>
inline bool TQuaternion<T>::operator == (const TQuaternion& rhs) const
{
	return this->isEqual(rhs);
}

template<typename T>
inline bool TQuaternion<T>::operator != (const TQuaternion& rhs) const
{
	return !(this->isEqual(rhs));
}

}// end namespace ph::math
