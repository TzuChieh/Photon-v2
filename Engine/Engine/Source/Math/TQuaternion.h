#pragma once

#include "Math/math_fwd.h"

#include <Common/primitive_type.h>

#include <cmath>
#include <string>
#include <array>

namespace ph::math
{

template<typename T>
class TQuaternion final
{
public:
	static TQuaternion makeNoRotation();

public:
	T x;
	T y;
	T z;
	T w;

public:
	inline TQuaternion() = default;
	inline TQuaternion(const TQuaternion& other) = default;

	TQuaternion(T x, T y, T z, T w);
	TQuaternion(const TVector3<T>& normalizedAxis, T radians);
	explicit TQuaternion(const TMatrix4<T>& rotationMatrix);

	template<typename U>
	explicit TQuaternion(const TQuaternion<U>& other);

	template<typename U>
	explicit TQuaternion(const std::array<U, 4>& xyzwValues);

	// acting like w component is 0
	TQuaternion mul(const TVector3<T>& xyz) const;
	void mul(const TVector3<T>& xyz, TQuaternion* out_result) const;

	TQuaternion normalize() const;
	TQuaternion& normalizeLocal();
	T length() const;
	TQuaternion conjugate() const;
	void conjugate(TQuaternion* out_result) const;
	TQuaternion& conjugateLocal();
	TQuaternion mul(const TQuaternion& rhs) const;
	void mul(const TQuaternion& rhs, TQuaternion* out_result) const;
	TQuaternion& mulLocal(const TQuaternion& rhs);
	TQuaternion mul(T rhs) const;
	TQuaternion sub(const TQuaternion& rhs) const;
	TQuaternion add(const TQuaternion& rhs) const;
	T dot(const TQuaternion& rhs) const;

	TQuaternion& set(T rhsX, T rhsY, T rhsZ, T rhsW);
	TQuaternion& set(const TQuaternion& rhs);
	void setRot(const TVector3<T>& normalizedAxis, T radians);
	void toRotationMatrix(TMatrix4<T>* out_result) const;

	std::string toString() const;

	bool isEqual(const TQuaternion& other) const;

	bool operator == (const TQuaternion& rhs) const;
	bool operator != (const TQuaternion& rhs) const;
};

}// end namespace ph::math

#include "Math/TQuaternion.ipp"
