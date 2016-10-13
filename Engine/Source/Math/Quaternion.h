#pragma once

#include "Common/type.h"

#include <cmath>

namespace pho
{

class Vector3f;
class Matrix4f;

class Quaternion
{
public:
	float32 x;
	float32 y;
	float32 z;
	float32 w;

public:
	Quaternion(const Quaternion& other) : x(other.x), y(other.y), z(other.z), w(other.w) {}
	Quaternion(float32 x, float32 y, float32 z, float32 w) : x(x), y(y), z(z), w(w) {}
	Quaternion() : x(0.0f), y(0.0f), z(0.0f), w(0.0f) {}

	Quaternion(const Vector3f& normalizedAxis, const float32 angle);

	// acting like w component is 0
	Quaternion mul(const Vector3f& xyz) const;
	void mul(const Vector3f& xyz, Quaternion* out_result) const;

	void setRot(const Vector3f& normalizedAxis, const float32 angle);
	void toRotationMatrix(Matrix4f* result) const;

	Quaternion nlerp(const Quaternion& dest, float32 lerpFactor, bool shortest) const;
	Quaternion slerp(const Quaternion& dest, float32 lerpFactor, bool shortest) const;

	inline Quaternion normalize() const
	{
		const float32 len = length();

		return Quaternion(x / len, y / len, z / len, w / len);
	}

	inline Quaternion& normalizeLocal()
	{
		const float32 len = length();

		x /= len;
		y /= len;
		z /= len;
		w /= len;

		return *this;
	}

	inline float32 length() const
	{
		return sqrt(x * x + y * y + z * z + w * w);
	}

	inline Quaternion conjugate() const
	{
		return Quaternion(-x, -y, -z, w);
	}

	inline void conjugate(Quaternion* result) const
	{
		result->x = -x;
		result->y = -y;
		result->z = -z;
		result->w = w;
	}

	inline Quaternion& conjugateLocal()
	{
		x *= -1;
		y *= -1;
		z *= -1;

		return *this;
	}

	inline Quaternion mul(const Quaternion& r) const
	{
		float32 _x = w * r.x + x * r.w + y * r.z - z * r.y;
		float32 _y = w * r.y - x * r.z + y * r.w + z * r.x;
		float32 _z = w * r.z + x * r.y - y * r.x + z * r.w;
		float32 _w = w * r.w - x * r.x - y * r.y - z * r.z;

		return Quaternion(_x, _y, _z, _w);
	}

	inline void mul(const Quaternion& r, Quaternion* result) const
	{
		result->x = w * r.x + x * r.w + y * r.z - z * r.y;
		result->y = w * r.y - x * r.z + y * r.w + z * r.x;
		result->z = w * r.z + x * r.y - y * r.x + z * r.w;
		result->w = w * r.w - x * r.x - y * r.y - z * r.z;
	}

	inline Quaternion& mulLocal(const Quaternion& r)
	{
		float32 _x = w * r.x + x * r.w + y * r.z - z * r.y;
		float32 _y = w * r.y - x * r.z + y * r.w + z * r.x;
		float32 _z = w * r.z + x * r.y - y * r.x + z * r.w;
		float32 _w = w * r.w - x * r.x - y * r.y - z * r.z;

		x = _x;
		y = _y;
		z = _z;
		w = _w;

		return *this;
	}

	inline Quaternion mul(const float r) const
	{
		return Quaternion(x * r, y * r, z * r, w * r);
	}

	inline Quaternion sub(const Quaternion& r) const
	{
		return Quaternion(x - r.x, y - r.y, z - r.z, w - r.w);
	}

	inline Quaternion add(const Quaternion& r) const
	{
		return Quaternion(x + r.x, y + r.y, z + r.z, w + r.w);
	}

	inline float32 dot(const Quaternion& r) const
	{
		return x * r.x + y * r.y + z * r.z + w * r.w;
	}

	/*public Vector3f getForward()
	{
		return Vector3f.UNIT_NEGATIVE_Z_AXIS.rotate(this);
	}

	public Vector3f getBack()
	{
		return Vector3f.UNIT_Z_AXIS.rotate(this);
	}

	public Vector3f getUp()
	{
		return Vector3f.UNIT_Y_AXIS.rotate(this);
	}

	public Vector3f getDown()
	{
		return Vector3f.UNIT_NEGATIVE_Y_AXIS.rotate(this);
	}

	public Vector3f getRight()
	{
		return Vector3f.UNIT_X_AXIS.rotate(this);
	}

	public Vector3f getLeft()
	{
		return Vector3f.UNIT_NEGATIVE_X_AXIS.rotate(this);
	}*/

	inline Quaternion& set(float32 rx, float32 ry, float32 rz, float32 rw)
	{
		this->x = rx;
		this->y = ry;
		this->z = rz;
		this->w = rw;

		return *this;
	}

	inline Quaternion& set(const Quaternion& r)
	{
		this->x = r.x;
		this->y = r.y;
		this->z = r.z;
		this->w = r.w;

		return *this;
	}
};// end class Quaternion

}// end namespace pho