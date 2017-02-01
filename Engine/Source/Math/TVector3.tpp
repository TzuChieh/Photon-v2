#pragma once

#include "Math/TVector3.h"
#include "Math/TQuaternion.h"
#include "FileIO/Tokenizer.h"

#include <cmath>
#include <string>
#include <cstdlib>
#include <iostream>

namespace ph
{

template<typename T>
inline TVector3<T>::TVector3() :
	x(0), y(0), z(0)
{

}

template<typename T>
inline TVector3<T>::TVector3(const T x, const T y, const T z) :
	x(x), y(y), z(z)
{

}

template<typename T>
inline TVector3<T>::TVector3(const T var) :
	x(var), y(var), z(var) 
{

}

template<typename T>
inline TVector3<T>::TVector3(const TVector3& other) :
	x(other.x), y(other.y), z(other.z)
{

}

template<typename T>
inline TVector3<T>::TVector3(const std::string& stringRepresentation) :
	TVector3()
{
	const Tokenizer tokenizer({' ', '\t', '\n', '\r'}, {});
	std::vector<std::string> tokens;
	tokenizer.tokenize(stringRepresentation, tokens);

	if(tokens.size() != 3)
	{
		std::cerr << "warning: at TVector3::TVector3(), bad string representation <" + stringRepresentation + ">" << std::endl;
		return;
	}

	x = static_cast<T>(std::stold(tokens[0]));
	y = static_cast<T>(std::stold(tokens[1]));
	z = static_cast<T>(std::stold(tokens[2]));
}

template<typename T>
inline TVector3<T>::~TVector3()
{

}

template<typename T>
TVector3<T> TVector3<T>::rotate(const TQuaternion<T>& rotation) const
{
	const TQuaternion<T>& conjugatedRotation = rotation.conjugate();
	const TQuaternion<T>& result = rotation.mul(*this).mulLocal(conjugatedRotation);

	return TVector3(result.x, result.y, result.z);
}

template<typename T>
void TVector3<T>::rotate(const TQuaternion<T>& rotation, TVector3* const out_result) const
{
	const TQuaternion<T>& conjugatedRotation = rotation.conjugate();
	const TQuaternion<T>& result = rotation.mul(*this).mulLocal(conjugatedRotation);

	out_result->x = result.x;
	out_result->y = result.y;
	out_result->z = result.z;
}

template<typename T>
inline T TVector3<T>::length() const
{
	return std::sqrt(squaredLength());
}

template<typename T>
inline T TVector3<T>::squaredLength() const
{
	return x * x + y * y + z * z;
}

template<typename T>
inline T TVector3<T>::max() const
{
	return fmax(x, fmax(y, z));
}

template<typename T>
inline T TVector3<T>::absMax() const
{
	return fmax(fabs(x), fmax(fabs(y), fabs(z)));
}

template<typename T>
inline TVector3<T> TVector3<T>::max(const TVector3& rhs) const
{
	return TVector3(fmax(x, rhs.x), fmax(y, rhs.y), fmax(z, rhs.z));
}

template<typename T>
inline TVector3<T>& TVector3<T>::maxLocal(const TVector3& rhs)
{
	x = fmax(x, rhs.x);
	y = fmax(y, rhs.y);
	z = fmax(z, rhs.z);

	return *this;
}

template<typename T>
inline TVector3<T> TVector3<T>::min(const TVector3& rhs) const
{
	return TVector3(fmin(x, rhs.x), fmin(y, rhs.y), fmin(z, rhs.z));
}

template<typename T>
inline TVector3<T>& TVector3<T>::minLocal(const TVector3& rhs)
{
	x = fmin(x, rhs.x);
	y = fmin(y, rhs.y);
	z = fmin(z, rhs.z);

	return *this;
}

template<typename T>
inline T TVector3<T>::dot(const TVector3& rhs) const
{
	return x * rhs.x + y * rhs.y + z * rhs.z;
}

template<typename T>
inline T TVector3<T>::absDot(const TVector3& rhs) const
{
	return fabs(dot(rhs));
}

template<typename T>
inline T TVector3<T>::dot(const T rhs) const
{
	return (x + y + z) * rhs;
}

template<typename T>
inline T TVector3<T>::dot(const T rhsX, const T rhsY, const T rhsZ) const
{
	return x * rhsX + y * rhsY + z * rhsZ;
}

template<typename T>
inline TVector3<T> TVector3<T>::cross(const TVector3& rhs) const
{
	return TVector3(y * rhs.z - z * rhs.y,
	                z * rhs.x - x * rhs.z,
	                x * rhs.y - y * rhs.x);
}

template<typename T>
inline void TVector3<T>::cross(const TVector3& rhs, TVector3* const out_result) const
{
	out_result->x = y * rhs.z - z * rhs.y;
	out_result->y = z * rhs.x - x * rhs.z;
	out_result->z = x * rhs.y - y * rhs.x;
}

template<typename T>
inline TVector3<T> TVector3<T>::normalize() const
{
	const T reciLen = 1 / length();
	return TVector3(x * reciLen, y * reciLen, z * reciLen);
}

template<typename T>
inline TVector3<T>& TVector3<T>::normalizeLocal()
{
	const T reciLen = 1 / length();

	x *= reciLen;
	y *= reciLen;
	z *= reciLen;

	return *this;
}

template<typename T>
inline TVector3<T> TVector3<T>::add(const TVector3& rhs) const
{
	return TVector3(x + rhs.x, y + rhs.y, z + rhs.z);
}

template<typename T>
inline void TVector3<T>::add(const TVector3& rhs, TVector3* const out_result) const
{
	out_result->x = x + rhs.x;
	out_result->y = y + rhs.y;
	out_result->z = z + rhs.z;
}

template<typename T>
inline TVector3<T> TVector3<T>::add(const T rhs) const
{
	return TVector3(x + rhs, y + rhs, z + rhs);
}

template<typename T>
inline TVector3<T>& TVector3<T>::addLocal(const TVector3& rhs)
{
	x += rhs.x;
	y += rhs.y;
	z += rhs.z;

	return *this;
}

template<typename T>
inline TVector3<T>& TVector3<T>::addLocal(const T rhs)
{
	x += rhs;
	y += rhs;
	z += rhs;

	return *this;
}

template<typename T>
inline TVector3<T>& TVector3<T>::addLocal(const T rhsX, const T rhsY, const T rhsZ)
{
	x += rhsX;
	y += rhsY;
	z += rhsZ;

	return *this;
}

template<typename T>
inline TVector3<T> TVector3<T>::sub(const TVector3& rhs) const
{
	return TVector3(x - rhs.x, y - rhs.y, z - rhs.z);
}

template<typename T>
inline TVector3<T> TVector3<T>::sub(const T rhs) const
{
	return TVector3(x - rhs, y - rhs, z - rhs);
}

template<typename T>
inline void TVector3<T>::sub(const TVector3& rhs, TVector3* const out_result) const
{
	out_result->x = x - rhs.x;
	out_result->y = y - rhs.y;
	out_result->z = z - rhs.z;
}

template<typename T>
inline TVector3<T>& TVector3<T>::subLocal(const TVector3& rhs)
{
	x -= rhs.x;
	y -= rhs.y;
	z -= rhs.z;

	return *this;
}

template<typename T>
inline TVector3<T>& TVector3<T>::subLocal(const T rhs)
{
	x -= rhs;
	y -= rhs;
	z -= rhs;

	return *this;
}

template<typename T>
inline TVector3<T> TVector3<T>::mul(const TVector3& rhs) const
{
	return TVector3(x * rhs.x, y * rhs.y, z * rhs.z);
}

template<typename T>
inline void TVector3<T>::mul(const TVector3& rhs, TVector3* const out_result) const
{
	out_result->x = x * rhs.x;
	out_result->y = y * rhs.y;
	out_result->z = z * rhs.z;
}

template<typename T>
inline TVector3<T> TVector3<T>::mul(const T rhs) const
{
	return TVector3(x * rhs, y * rhs, z * rhs);
}

template<typename T>
inline void TVector3<T>::mul(const T rhs, TVector3* const out_result) const
{
	out_result->x = x * rhs;
	out_result->y = y * rhs;
	out_result->z = z * rhs;
}

template<typename T>
inline TVector3<T>& TVector3<T>::mulLocal(const T rhs)
{
	x *= rhs;
	y *= rhs;
	z *= rhs;

	return *this;
}

template<typename T>
inline TVector3<T>& TVector3<T>::mulLocal(const TVector3& rhs)
{
	x *= rhs.x;
	y *= rhs.y;
	z *= rhs.z;

	return *this;
}

template<typename T>
inline TVector3<T>& TVector3<T>::maddLocal(const T multiplier, const TVector3& adder)
{
	x = x * multiplier + adder.x;
	y = y * multiplier + adder.y;
	z = x * multiplier + adder.z;

	return *this;
}

template<typename T>
inline TVector3<T>& TVector3<T>::negateLocal()
{
	x *= -1;
	y *= -1;
	z *= -1;

	return *this;
}

template<typename T>
inline TVector3<T> TVector3<T>::div(const TVector3& rhs) const
{
	return TVector3(x / rhs.x, y / rhs.y, z / rhs.z);
}

template<typename T>
inline TVector3<T>& TVector3<T>::divLocal(const TVector3& rhs)
{
	x /= rhs.x;
	y /= rhs.y;
	z /= rhs.z;

	return *this;
}

template<typename T>
inline TVector3<T> TVector3<T>::div(const T rhs) const
{
	return TVector3(x / rhs, y / rhs, z / rhs);
}

template<typename T>
inline TVector3<T>& TVector3<T>::divLocal(const T rhs)
{
	x /= rhs;
	y /= rhs;
	z /= rhs;

	return *this;
}

template<typename T>
inline TVector3<T> TVector3<T>::abs() const
{
	return TVector3(fabs(x), fabs(y), fabs(z));
}

template<typename T>
inline TVector3<T>& TVector3<T>::absLocal()
{
	x = fabs(x);
	y = fabs(y);
	z = fabs(z);

	return *this;
}

// TODO: move to a specilized utility function
// Current vector is expected to be normalized already.
template<typename T>
inline void TVector3<T>::calcOrthBasisAsYaxis(TVector3* const out_xAxis, TVector3* const out_zAxis) const
{
	if(std::abs(y) < 0.9f)
	{
		out_xAxis->set(-z, 0.0f, x);// yAxis cross (0, 1, 0)
		out_xAxis->divLocal(sqrt(x * x + z * z));
	}
	else
	{
		out_xAxis->set(y, -x, 0.0f);// yAxis cross (0, 0, 1)
		out_xAxis->divLocal(sqrt(x * x + y * y));
	}

	out_xAxis->cross(*this, out_zAxis);
}

// Clamp current vector's components to specific range. If a component is NaN, its value is
// clamped to lower bound. Either lower bound or upper bound shall not be NaN, or the method's 
// behavior is undefined.

template<typename T>
inline TVector3<T> TVector3<T>::clamp(const T lowerBound, const T upperBound) const
{
	return TVector3(fmin(upperBound, fmax(x, lowerBound)),
	                fmin(upperBound, fmax(y, lowerBound)),
	                fmin(upperBound, fmax(z, lowerBound)));
}

template<typename T>
inline TVector3<T>& TVector3<T>::clampLocal(const T lowerBound, const T upperBound)
{
	x = fmin(upperBound, fmax(x, lowerBound));
	y = fmin(upperBound, fmax(y, lowerBound));
	z = fmin(upperBound, fmax(z, lowerBound));

	return *this;
}

template<typename T>
inline TVector3<T> TVector3<T>::complement() const
{
	return TVector3(1 - x, 1 - y, 1 - z);
}

template<typename T>
inline T TVector3<T>::avg() const
{
	return (x + y + z) / 3;
}

template<typename T>
inline TVector3<T> TVector3<T>::reciprocal() const
{
	return TVector3(1 / x, 1 / y, 1 / z);
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

	x = x - factor * normal.x;
	y = y - factor * normal.y;
	z = z - factor * normal.z;

	return *this;
}

// returned (x, y, z) = (min, mid, max)
template<typename T>
inline void TVector3<T>::sort(TVector3* const out_result) const
{
	if(x > y)
	{
		if(x > z)
		{
			out_result->z = x;

			if(y < z)
			{
				out_result->x = y;
				out_result->y = z;
			}
			else
			{
				out_result->x = z;
				out_result->y = y;
			}
		}
		else
		{
			out_result->z = z;
			out_result->y = x;
			out_result->x = y;
		}
	}
	else
	{
		if(x < z)
		{
			out_result->x = x;

			if(y > z)
			{
				out_result->z = y;
				out_result->y = z;
			}
			else
			{
				out_result->z = z;
				out_result->y = y;
			}
		}
		else
		{
			out_result->x = z;
			out_result->y = x;
			out_result->z = y;
		}
	}
}

template<typename T>
inline std::string TVector3<T>::toStringFormal() const
{
	return "[" + std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(z) + "]";
}

template<typename T>
inline TVector3<T>& TVector3<T>::set(const T rhsX, const T rhsY, const T rhsZ)
{
	x = rhsX;
	y = rhsY;
	z = rhsZ;

	return *this;
}

template<typename T>
inline TVector3<T>& TVector3<T>::set(const T rhs)
{
	x = rhs;
	y = rhs;
	z = rhs;

	return *this;
}

template<typename T>
inline TVector3<T>& TVector3<T>::set(const TVector3& rhs)
{
	return set(rhs.x, rhs.y, rhs.z);
}

template<typename T>
inline void TVector3<T>::set(TVector3* const out_value) const
{
	out_value->x = x;
	out_value->y = y;
	out_value->z = z;
}

template<typename T>
inline bool TVector3<T>::equals(const TVector3& rhs) const
{
	return x == rhs.x &&
	       y == rhs.y &&
	       z == rhs.z;
}

template<typename T>
inline bool TVector3<T>::equals(const TVector3& rhs, const T margin) const
{
	return (x - rhs.x) < std::abs(margin) &&
	       (y - rhs.y) < std::abs(margin) &&
	       (z - rhs.z) < std::abs(margin);
}

template<typename T>
inline bool TVector3<T>::isZero() const
{
	return x == 0 && y == 0 && z == 0;
}

}// end namespace ph