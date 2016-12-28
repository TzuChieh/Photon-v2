#pragma once

#include "Common/primitive_type.h"

#include <cmath>
#include <string>
#include <cstdlib>

namespace ph
{

class Quaternion;

class Vector3f
{
public:
	static const Vector3f UNIT_X_AXIS;
	static const Vector3f UNIT_Y_AXIS;
	static const Vector3f UNIT_Z_AXIS;
	static const Vector3f UNIT_NEGATIVE_X_AXIS;
	static const Vector3f UNIT_NEGATIVE_Y_AXIS;
	static const Vector3f UNIT_NEGATIVE_Z_AXIS;

public:
	float32 x;
	float32 y;
	float32 z;

public:
	inline Vector3f() : x(0.0f), y(0.0f), z(0.0f) {}
	inline Vector3f(const float32 x, const float32 y, const float32 z) : x(x), y(y), z(z) {}
	inline explicit Vector3f(const float32 var) : x(var), y(var), z(var) {}
	inline Vector3f(const Vector3f& other) : x(other.x), y(other.y), z(other.z) {}
	inline ~Vector3f() {}

	Vector3f rotate(const Quaternion& rotation) const;
	void rotate(const Quaternion& rotation, Vector3f* out_result) const;

	inline float32 length() const
	{
		return sqrt(x * x + y * y + z * z);
	}

	inline float32 squaredLength() const
	{
		return x * x + y * y + z * z;
	}

	inline float32 max() const
	{
		return fmax(x, fmax(y, z));
	}

	inline float32 absMax() const
	{
		return fmax(fabs(x), fmax(fabs(y), fabs(z)));
	}

	inline Vector3f max(const Vector3f& r) const
	{
		return Vector3f(fmax(x, r.x), fmax(y, r.y), fmax(z, r.z));
	}

	inline Vector3f& maxLocal(const Vector3f& r)
	{
		x = fmax(x, r.x);
		y = fmax(y, r.y);
		z = fmax(z, r.z);

		return *this;
	}

	inline Vector3f min(const Vector3f& r) const
	{
		return Vector3f(fmin(x, r.x), fmin(y, r.y), fmin(z, r.z));
	}

	inline Vector3f& minLocal(const Vector3f& r)
	{
		x = fmin(x, r.x);
		y = fmin(y, r.y);
		z = fmin(z, r.z);

		return *this;
	}

	inline float32 dot(const Vector3f& r) const
	{
		return x * r.x + y * r.y + z * r.z;
	}

	inline float32 absDot(const Vector3f& r) const
	{
		return fabs(dot(r));
	}

	inline float32 dot(const float32 r) const
	{
		return (x + y + z) * r;
	}

	inline float32 dot(const float32 rX, const float32 rY, const float32 rZ) const
	{
		return x * rX + y * rY + z * rZ;
	}

	inline Vector3f cross(const Vector3f& r) const
	{
		return Vector3f(y * r.z - z * r.y, 
			            z * r.x - x * r.z, 
			            x * r.y - y * r.x);
	}

	inline void cross(const Vector3f& value, Vector3f* result) const
	{
		result->x = y * value.z - z * value.y;
		result->y = z * value.x - x * value.z;
		result->z = x * value.y - y * value.x;
	}

	inline Vector3f& setLength(const float32 len)
	{
		const float32 multiplier = len / length();

		x *= multiplier;
		y *= multiplier;
		z *= multiplier;

		return *this;
	}

	inline Vector3f normalize() const
	{
		const float32 reciprocalLen = 1.0f / length();
		return Vector3f(x * reciprocalLen, y * reciprocalLen, z * reciprocalLen);
	}

	inline Vector3f& normalizeLocal()
	{
		const float32 reciprocalLen = 1.0f / length();

		x *= reciprocalLen;
		y *= reciprocalLen;
		z *= reciprocalLen;

		return *this;
	}

	/*public Vector3f rotate(Quaternion rotation)
	{
		rotation.conjugate(Quaternion.TEMP);

		Quaternion w = rotation.mul(this).mulLocal(Quaternion.TEMP);

		return new Vector3f(w.getX(), w.getY(), w.getZ());
	}*/

	inline Vector3f lerp(const Vector3f& destination, const float32 lerpFactor) const
	{
		return destination.sub(*this).mulLocal(lerpFactor).addLocal(*this);
	}

	inline Vector3f& setLinearInterpolated(const Vector3f& start, const Vector3f& end, const float fraction)
	{
		x = (end.x - start.x) * fraction + start.x;
		y = (end.y - start.y) * fraction + start.y;
		z = (end.z - start.z) * fraction + start.z;

		return *this;
	}

	inline Vector3f add(const Vector3f& r) const
	{
		return Vector3f(x + r.x, y + r.y, z + r.z);
	}

	inline void add(const Vector3f& var, Vector3f* result) const
	{
		result->x = x + var.x;
		result->y = y + var.y;
		result->z = z + var.z;
	}

	inline Vector3f add(const float32 r) const
	{
		return Vector3f(x + r, y + r, z + r);
	}

	inline Vector3f& addLocal(const Vector3f& r)
	{
		x += r.x;
		y += r.y;
		z += r.z;

		return *this;
	}

	inline Vector3f& addLocal(const float32 r)
	{
		x += r;
		y += r;
		z += r;

		return *this;
	}

	inline Vector3f& addLocal(const float32 rX, const float32 rY, const float32 rZ)
	{
		x += rX;
		y += rY;
		z += rZ;

		return *this;
	}
		
	inline Vector3f sub(const Vector3f& r) const
	{
		return Vector3f(x - r.x, y - r.y, z - r.z);
	}

	inline Vector3f sub(const float32 r) const
	{
		return Vector3f(x - r, y - r, z - r);
	}

	inline void sub(const Vector3f& var, Vector3f* result) const
	{
		result->x = x - var.x;
		result->y = y - var.y;
		result->z = z - var.z;
	}

	inline Vector3f& subLocal(const Vector3f& r)
	{
		x -= r.x;
		y -= r.y;
		z -= r.z;

		return *this;
	}

	inline Vector3f& subLocal(const float32 r)
	{
		x -= r;
		y -= r;
		z -= r;

		return *this;
	}

	inline Vector3f mul(const Vector3f& r) const
	{
		return Vector3f(x * r.x, y * r.y, z * r.z);
	}

	inline void mul(const Vector3f& r, Vector3f* result) const
	{
		result->x = x * r.x;
		result->y = y * r.y;
		result->z = z * r.z;
	}

	inline Vector3f mul(const float32 var) const
	{
		return Vector3f(x * var, y * var, z * var);
	}

	inline void mul(const float32 r, Vector3f *result) const
	{
		result->x = x * r;
		result->y = y * r;
		result->z = z * r;
	}

	inline Vector3f& mulLocal(const float32 r)
	{
		x *= r;
		y *= r;
		z *= r;

		return *this;
	}

	inline Vector3f& mulLocal(const Vector3f& r)
	{
		x *= r.x;
		y *= r.y;
		z *= r.z;

		return *this;
	}

	inline Vector3f& maddLocal(float32 multiplier, const Vector3f& adder)
	{
		x = x * multiplier + adder.x;
		y = y * multiplier + adder.y;
		z = x * multiplier + adder.z;

		return *this;
	}

	inline Vector3f& negateLocal()
	{
		x *= -1;
		y *= -1;
		z *= -1;

		return *this;
	}

	inline Vector3f div(const Vector3f& r) const
	{
		return Vector3f(x / r.x, y / r.y, z / r.z);
	}

	inline Vector3f div(const float32 r) const
	{
		return Vector3f(x / r, y / r, z / r);
	}

	inline Vector3f& divLocal(const float32 r)
	{
		x /= r;
		y /= r;
		z /= r;

		return *this;
	}

	inline Vector3f& divLocal(const Vector3f& r)
	{
		x /= r.x;
		y /= r.y;
		z /= r.z;

		return *this;
	}

	inline Vector3f abs() const
	{
		return Vector3f(fabs(x), fabs(y), fabs(z));
	}

	inline Vector3f& absLocal()
	{
		x = fabs(x);
		y = fabs(y);
		z = fabs(z);

		return *this;
	}

	// Current vector is expected to be normalized already.
	inline void calcOrthBasisAsYaxis(Vector3f* out_xAxis, Vector3f* out_zAxis) const
	{
		if(fabs(y) < 0.9f)
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

	inline Vector3f clamp(const float32 lowerBound, const float32 upperBound)
	{
		return Vector3f(fmin(upperBound, fmax(x, lowerBound)), 
		                fmin(upperBound, fmax(y, lowerBound)), 
		                fmin(upperBound, fmax(z, lowerBound)));
	}

	inline Vector3f& clampLocal(const float32 lowerBound, const float32 upperBound)
	{
		x = fmin(upperBound, fmax(x, lowerBound));
		y = fmin(upperBound, fmax(y, lowerBound));
		z = fmin(upperBound, fmax(z, lowerBound));

		return *this;
	}

	inline Vector3f complement() const
	{
		return Vector3f(1.0f - x, 1.0f - y, 1.0f - z);
	}

	inline float avg() const
	{
		return (x + y + z) / 3.0f;
	}

	inline Vector3f reciprocal() const
	{
		return Vector3f(1.0f / x, 1.0f / y, 1.0f / z);
	}

	inline Vector3f reflect(const Vector3f& normal) const
	{
		Vector3f result = normal.mul(-2.0f * normal.dot(*this));
		result.addLocal(*this);

		return result;
	}

	inline Vector3f& reflectLocal(const Vector3f& normal)
	{
		float32 factor = -2.0f * normal.dot(*this);

		x = x + factor * normal.x;
		y = y + factor * normal.y;
		z = z + factor * normal.z;

		return *this;
	}

	// returned (x, y, z) = (min, mid, max)
	inline void sort(Vector3f* result) const
	{
		if(x > y)
		{
			if(x > z)
			{
				result->z = x;

				if(y < z)
				{
					result->x = y;
					result->y = z;
				}
				else
				{
					result->x = z;
					result->y = y;
				}
			}
			else
			{
				result->z = z;
				result->y = x;
				result->x = y;
			}
		}
		else
		{
			if(x < z)
			{
				result->x = x;

				if(y > z)
				{
					result->z = y;
					result->y = z;
				}
				else
				{
					result->z = z;
					result->y = y;
				}
			}
			else
			{
				result->x = z;
				result->y = x;
				result->z = y;
			}
		}
	}

	inline std::string toStringFormal() const
	{
		return "[" + std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(z) + "]";
	}

	inline Vector3f& set(const float32 rx, const float32 ry, const float32 rz)
	{
		this->x = rx;
		this->y = ry;
		this->z = rz;
		
		return *this;
	}

	inline Vector3f& set(const float32 r)
	{
		this->x = r;
		this->y = r;
		this->z = r;

		return *this;
	}

	inline Vector3f& set(const Vector3f& r)
	{
		return set(r.x, r.y, r.z);
	}

	inline void set(Vector3f* const out_value) const
	{
		out_value->x = x;
		out_value->y = y;
		out_value->z = z;
	}

	inline bool equals(const Vector3f& r) const
	{
		return x == r.x &&
		       y == r.y &&
		       z == r.z;
	}

	inline bool equals(const Vector3f& r, const float32 margin) const
	{
		return (x - r.x) < std::abs(margin) &&
		       (y - r.y) < std::abs(margin) &&
		       (z - r.z) < std::abs(margin);
	}
};// end class Vector3f

}// end namespace ph