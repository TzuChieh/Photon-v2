#pragma once

#include "Math/TQuaternion.h"
#include "Math/TVector3.h"
#include "Math/TMatrix4.h"
#include "Math/math.h"

#include <Common/assertion.h>

namespace ph::math
{

template<typename T>
inline TQuaternion<T> TQuaternion<T>::makeNoRotation()
{
	return TQuaternion(0, 0, 0, 1);
}

template<typename T>
inline TQuaternion<T>::TQuaternion(const T vx, const T vy, const T vz, const T vw)
	: Base(std::array<T, 4>{vx, vy, vz, vw})
{}

template<typename T>
template<typename U>
inline TQuaternion<T>::TQuaternion(const TQuaternion<U>& other)
	: TQuaternion(
		static_cast<T>(other.x()), 
		static_cast<T>(other.y()),
		static_cast<T>(other.z()),
		static_cast<T>(other.w()))
{}

template<typename T>
template<typename U>
inline TQuaternion<T>::TQuaternion(const std::array<U, 4>& xyzwValues)
	: TQuaternion(
		TQuaternion<U>(xyzwValues[0], xyzwValues[1], xyzwValues[2], xyzwValues[3]))
{}

template<typename T>
inline TQuaternion<T>::TQuaternion(const TVector3<T>& normalizedAxis, const T radians)
	: TQuaternion()
{
	setRot(normalizedAxis, radians);
}

template<typename T>
inline TQuaternion<T>::TQuaternion(const TMatrix4<T>& rotationMatrix)
	: TQuaternion()
{
	const TMatrix4<T>& m = rotationMatrix;

	const T trace = m.m[0][0] + m.m[1][1] + m.m[2][2];
	if(trace > 0)
	{
		const T s = static_cast<T>(0.5) / std::sqrt(trace + 1);
		x() = (m.m[2][1] - m.m[1][2]) * s;
		y() = (m.m[0][2] - m.m[2][0]) * s;
		z() = (m.m[1][0] - m.m[0][1]) * s;
		w() = static_cast<T>(0.25) / s;
	}
	else
	{
		if(m.m[0][0] > m.m[1][1] && m.m[0][0] > m.m[2][2])
		{
			const T s = static_cast<T>(0.5) / std::sqrt(1 + m.m[0][0] - m.m[1][1] - m.m[2][2]);
			x() = static_cast<T>(0.25) / s;
			y() = (m.m[0][1] + m.m[1][0]) * s;
			z() = (m.m[0][2] + m.m[2][0]) * s;
			w() = (m.m[2][1] - m.m[1][2]) * s;
		}
		else if(m.m[1][1] > m.m[2][2])
		{
			const T s = static_cast<T>(0.5) / std::sqrt(1 + m.m[1][1] - m.m[0][0] - m.m[2][2]);
			x() = (m.m[0][1] + m.m[1][0]) * s;
			y() = static_cast<T>(0.25) / s;
			z() = (m.m[1][2] + m.m[2][1]) * s;
			w() = (m.m[0][2] - m.m[2][0]) * s;
		}
		else
		{
			const T s = static_cast<T>(0.5) / std::sqrt(1 + m.m[2][2] - m.m[0][0] - m.m[1][1]);
			x() = (m.m[0][2] + m.m[2][0]) * s;
			y() = (m.m[1][2] + m.m[2][1]) * s;
			z() = static_cast<T>(0.25) / s;
			w() = (m.m[1][0] - m.m[0][1]) * s;
		}
	}

	normalizeLocal();
}

template<typename T>
inline T& TQuaternion<T>::x()
{
	return m[0];
}

template<typename T>
inline T& TQuaternion<T>::y()
{
	return m[1];
}

template<typename T>
inline T& TQuaternion<T>::z()
{
	return m[2];
}

template<typename T>
inline T& TQuaternion<T>::w()
{
	return m[3];
}

template<typename T>
inline const T& TQuaternion<T>::x() const
{
	return m[0];
}

template<typename T>
inline const T& TQuaternion<T>::y() const
{
	return m[1];
}

template<typename T>
inline const T& TQuaternion<T>::z() const
{
	return m[2];
}

template<typename T>
inline const T& TQuaternion<T>::w() const
{
	return m[3];
}

template<typename T>
inline TQuaternion<T> TQuaternion<T>::mul(const TVector3<T>& xyz) const
{
	// Acting like `xyz`'s w component is 0
	return TQuaternion(w() * xyz.x() + y() * xyz.z() - z() * xyz.y(),
	                   w() * xyz.y() - x() * xyz.z() + z() * xyz.x(),
	                   w() * xyz.z() + x() * xyz.y() - y() * xyz.x(),
	                  -x() * xyz.x() - y() * xyz.y() - z() * xyz.z());
}

template<typename T>
inline TQuaternion<T> TQuaternion<T>::normalize() const
{
	return TQuaternion(*this).normalizeLocal();
}

template<typename T>
inline TQuaternion<T>& TQuaternion<T>::normalizeLocal()
{
	::ph::math::normalize(m);

	return *this;
}

template<typename T>
inline T TQuaternion<T>::length() const
{
	return ::ph::math::length(m);
}

template<typename T>
inline TQuaternion<T> TQuaternion<T>::conjugate() const
{
	return TQuaternion(*this).conjugateLocal();
}

template<typename T>
inline void TQuaternion<T>::conjugate(TQuaternion* const out_result) const
{
	PH_ASSERT(out_result);

	*out_result = *this;
	out_result->conjugateLocal();
}

template<typename T>
inline TQuaternion<T>& TQuaternion<T>::conjugateLocal()
{
	x() *= -1;
	y() *= -1;
	z() *= -1;

	return *this;
}

template<typename T>
inline TQuaternion<T> TQuaternion<T>::mul(const TQuaternion& rhs) const
{
	return TQuaternion(*this).mulLocal(rhs);
}

template<typename T>
inline TQuaternion<T>& TQuaternion<T>::mulLocal(const TQuaternion& rhs)
{
	const T _x = w() * rhs.x() + x() * rhs.w() + y() * rhs.z() - z() * rhs.y();
	const T _y = w() * rhs.y() - x() * rhs.z() + y() * rhs.w() + z() * rhs.x();
	const T _z = w() * rhs.z() + x() * rhs.y() - y() * rhs.x() + z() * rhs.w();
	const T _w = w() * rhs.w() - x() * rhs.x() - y() * rhs.y() - z() * rhs.z();

	x() = _x;
	y() = _y;
	z() = _z;
	w() = _w;

	return *this;
}

template<typename T>
inline TQuaternion<T> TQuaternion<T>::mul(const T rhs) const
{
	return TQuaternion(x() * rhs, y() * rhs, z() * rhs, w() * rhs);
}

template<typename T>
inline TQuaternion<T> TQuaternion<T>::sub(const TQuaternion& rhs) const
{
	return TQuaternion(x() - rhs.x(), y() - rhs.y(), z() - rhs.z(), w() - rhs.w());
}

template<typename T>
inline TQuaternion<T> TQuaternion<T>::add(const TQuaternion& rhs) const
{
	return TQuaternion(x() + rhs.x(), y() + rhs.y(), z() + rhs.z(), w() + rhs.w());
}

template<typename T>
inline T TQuaternion<T>::dot(const TQuaternion& rhs) const
{
	return ::ph::math::dot_product(m, rhs.m);
}

template<typename T>
inline void TQuaternion<T>::setRot(const TVector3<T>& normalizedAxis, const T radians)
{
	const T sinHalfAngle = std::sin(radians / 2);
	const T cosHalfAngle = std::cos(radians / 2);

	x() = normalizedAxis.x() * sinHalfAngle;
	y() = normalizedAxis.y() * sinHalfAngle;
	z() = normalizedAxis.z() * sinHalfAngle;
	w() = cosHalfAngle;
}

template<typename T>
inline void TQuaternion<T>::toRotationMatrix(TMatrix4<T>* const out_result) const
{
	PH_ASSERT(out_result);

	out_result->m[0][0] = 1 - 2 * (y() * y() + z() * z());
	out_result->m[0][1] = 2 * (x() * y() - z() * w());
	out_result->m[0][2] = 2 * (y() * w() + x() * z());
	out_result->m[0][3] = 0;

	out_result->m[1][0] = 2 * (x() * y() + z() * w());
	out_result->m[1][1] = 1 - 2 * (x() * x() + z() * z());
	out_result->m[1][2] = 2 * (y() * z() - x() * w());
	out_result->m[1][3] = 0;

	out_result->m[2][0] = 2 * (x() * z() - y() * w());
	out_result->m[2][1] = 2 * (y() * z() + x() * w());
	out_result->m[2][2] = 1 - 2 * (x() * x() + y() * y());
	out_result->m[2][3] = 0;

	out_result->m[3][0] = 0;
	out_result->m[3][1] = 0;
	out_result->m[3][2] = 0;
	out_result->m[3][3] = 1;
}

#if PH_COMPILER_HAS_P2468R2

template<typename T>
inline bool TQuaternion<T>::operator == (const Self& other) const
{
	return Base::operator == (other);
}

#endif

}// end namespace ph::math
