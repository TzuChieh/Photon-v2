#pragma once

#include "Math/TVector3.h"
#include "Math/TQuaternion.h"
#include "Math/math.h"

#include <Common/assertion.h>

#include <array>
#include <cmath>
#include <string>
#include <cstdlib>
#include <algorithm>

namespace ph::math
{

template<typename T>
inline TVector3<T> TVector3<T>::weightedSum(
	const TVector3& vA, const T wA,
	const TVector3& vB, const T wB,
	const TVector3& vC, const T wC)
{
	return TVector3(vA.x() * wA + vB.x() * wB + vC.x() * wC,
	                vA.y() * wA + vB.y() * wB + vC.y() * wC,
	                vA.z() * wA + vB.z() * wB + vC.z() * wC);
}

template<typename T>
inline TVector3<T> TVector3<T>::lerp(const TVector3& vA, const TVector3& vB, const T parametricT)
{
	const T oneMinusT = 1 - parametricT;
	return TVector3(vA.x() * oneMinusT + vB.x() * parametricT,
	                vA.y() * oneMinusT + vB.y() * parametricT,
	                vA.z() * oneMinusT + vB.z() * parametricT);
}

template<typename T>
inline TVector3<T>::TVector3(const T vx, const T vy, const T vz) : 
	Base(std::array<T, 3>{vx, vy, vz})
{}

template<typename T>
template<typename U>
inline TVector3<T>::TVector3(const TVector3<U>& other) : 
	TVector3(
		static_cast<T>(other.x()), 
		static_cast<T>(other.y()),
		static_cast<T>(other.z()))
{}

template<typename T>
inline TVector3<T> TVector3<T>::rotate(const TQuaternion<T>& rotation) const
{
	const TQuaternion<T>& conjugatedRotation = rotation.conjugate();
	const TQuaternion<T> result = rotation.mul(*this).mulLocal(conjugatedRotation);

	return TVector3(result.x(), result.y(), result.z());
}

template<typename T>
inline void TVector3<T>::rotate(const TQuaternion<T>& rotation,
                                TVector3* const out_result) const
{
	PH_ASSERT(out_result);
	PH_ASSERT(out_result != this);

	const TQuaternion<T>& conjugatedRotation = rotation.conjugate();
	const TQuaternion<T> result = rotation.mul(*this).mulLocal(conjugatedRotation);

	out_result->x() = result.x();
	out_result->y() = result.y();
	out_result->z() = result.z();
}

template<typename T>
inline TVector3<T> TVector3<T>::cross(const TVector3& rhs) const
{
	return TVector3(y() * rhs.z() - z() * rhs.y(),
	                z() * rhs.x() - x() * rhs.z(),
	                x() * rhs.y() - y() * rhs.x());
}

template<typename T>
inline void TVector3<T>::cross(const TVector3& rhs, 
                               TVector3* const out_result) const
{
	PH_ASSERT(out_result);
	PH_ASSERT(out_result != this);

	out_result->x() = y() * rhs.z() - z() * rhs.y();
	out_result->y() = z() * rhs.x() - x() * rhs.z();
	out_result->z() = x() * rhs.y() - y() * rhs.x();
}

template<typename T>
inline TVector3<T>& TVector3<T>::maddLocal(const T multiplier, 
                                           const TVector3& adder)
{
	x() = x() * multiplier + adder.x();
	y() = y() * multiplier + adder.y();
	z() = x() * multiplier + adder.z();

	return *this;
}

template<typename T>
inline TVector3<T> TVector3<T>::reflect(const TVector3& normal) const
{
	TVector3 result = normal.mul(2 * normal.dot(*this));
	return this->sub(result);
}

template<typename T>
inline TVector3<T>& TVector3<T>::reflectLocal(const TVector3& normal)
{
	const T factor = 2 * normal.dot(*this);

	x() -= factor * normal.x();
	y() -= factor * normal.y();
	z() -= factor * normal.z();

	return *this;
}

template<typename T>
inline void TVector3<T>::sort(TVector3* const out_result) const
{
	PH_ASSERT(out_result);
	PH_ASSERT(out_result != this);

	// Returned (x, y, z) = (min, mid, max)

	if(x() > y())
	{
		if(x() > z())
		{
			out_result->z() = x();

			if(y() < z())
			{
				out_result->x() = y();
				out_result->y() = z();
			}
			else
			{
				out_result->x() = z();
				out_result->y() = y();
			}
		}
		else
		{
			out_result->z() = z();
			out_result->y() = x();
			out_result->x() = y();
		}
	}
	else
	{
		if(x() < z())
		{
			out_result->x() = x();

			if(y() > z())
			{
				out_result->z() = y();
				out_result->y() = z();
			}
			else
			{
				out_result->z() = z();
				out_result->y() = y();
			}
		}
		else
		{
			out_result->x() = z();
			out_result->y() = x();
			out_result->z() = y();
		}
	}
}

template<typename T>
inline TVector3<T> operator * (const T rhs, const TVector3<T>& lhs)
{
	return lhs.mul(rhs);
}

template<typename T>
inline T& TVector3<T>::x()
{
	return m[0];
}

template<typename T>
inline T& TVector3<T>::y()
{
	return m[1];
}

template<typename T>
inline T& TVector3<T>::z()
{
	return m[2];
}

template<typename T>
inline const T& TVector3<T>::x() const
{
	return m[0];
}

template<typename T>
inline const T& TVector3<T>::y() const
{
	return m[1];
}

template<typename T>
inline const T& TVector3<T>::z() const
{
	return m[2];
}

template<typename T>
inline T& TVector3<T>::r()
{
	return m[0];
}

template<typename T>
inline T& TVector3<T>::g()
{
	return m[1];
}

template<typename T>
inline T& TVector3<T>::b()
{
	return m[2];
}

template<typename T>
inline const T& TVector3<T>::r() const
{
	return m[0];
}

template<typename T>
inline const T& TVector3<T>::g() const
{
	return m[1];
}

template<typename T>
inline const T& TVector3<T>::b() const
{
	return m[2];
}

}// end namespace ph::math
