#pragma once

#include "Common/primitive_type.h"
#include "Math/math_fwd.h"

#include <cmath>
#include <string>

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

	template<typename U>
	inline explicit TQuaternion(const TQuaternion<U>& other);

	inline TQuaternion(const T x, const T y, const T z, const T w);
	inline TQuaternion(const TVector3<T>& normalizedAxis, const T radians);
	inline explicit TQuaternion(const TMatrix4<T>& rotationMatrix);

	// acting like w component is 0
	inline TQuaternion mul(const TVector3<T>& xyz) const;
	inline void mul(const TVector3<T>& xyz, TQuaternion* const out_result) const;

	inline TQuaternion normalize() const;
	inline TQuaternion& normalizeLocal();
	inline T length() const;
	inline TQuaternion conjugate() const;
	inline void conjugate(TQuaternion* const out_result) const;
	inline TQuaternion& conjugateLocal();
	inline TQuaternion mul(const TQuaternion& rhs) const;
	inline void mul(const TQuaternion& rhs, TQuaternion* const out_result) const;
	inline TQuaternion& mulLocal(const TQuaternion& rhs);
	inline TQuaternion mul(const T rhs) const;
	inline TQuaternion sub(const TQuaternion& rhs) const;
	inline TQuaternion add(const TQuaternion& rhs) const;
	inline T dot(const TQuaternion& rhs) const;

	inline TQuaternion& set(const T rhsX, const T rhsY, const T rhsZ, const T rhsW);
	inline TQuaternion& set(const TQuaternion& rhs);
	inline void setRot(const TVector3<T>& normalizedAxis, const T radians);
	inline void toRotationMatrix(TMatrix4<T>* const out_result) const;

	inline std::string toString() const;
};

}// end namespace ph::math

#include "Math/TQuaternion.ipp"
