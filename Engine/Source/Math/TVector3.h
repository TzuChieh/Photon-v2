#pragma once

#include "Common/primitive_type.h"
#include "Math/math_fwd.h"

#include <cmath>
#include <string>
#include <cstdlib>

namespace ph
{

template<typename T>
class TVector3
{
public:
	T x;
	T y;
	T z;

public:
	inline TVector3();
	inline TVector3(const T x, const T y, const T z);
	inline explicit TVector3(const T var);
	inline explicit TVector3(const std::string& stringRepresentation);
	inline TVector3(const TVector3& other);
	inline ~TVector3();

	Vector3R rotate(const Quaternion& rotation) const;
	void rotate(const Quaternion& rotation, Vector3R* const out_result) const;

	inline real length() const;
	inline real squaredLength() const;

	inline T max() const;
	inline T absMax() const;
	inline TVector3 max(const TVector3& rhs) const;
	inline TVector3& maxLocal(const TVector3& rhs);
	inline TVector3 min(const TVector3& rhs) const;
	inline TVector3& minLocal(const TVector3& rhs);

	inline T dot(const TVector3& rhs) const;
	inline T absDot(const TVector3& rhs) const;
	inline T dot(const T rhs) const;
	inline T dot(const T rhsX, const T rhsY, const T rhsZ) const;

	inline TVector3 cross(const TVector3& rhs) const;
	inline void cross(const TVector3& rhs, TVector3* const out_result) const;

	inline TVector3 normalize() const;
	inline TVector3& normalizeLocal();

	/*inline Vector3R lerp(const TVector3& destination, const real lerpFactor) const
	{
		return destination.sub(*this).mulLocal(lerpFactor).addLocal(*this);
	}

	inline Vector3f& setLinearInterpolated(const Vector3f& start, const Vector3f& end, const float fraction)
	{
		x = (end.x - start.x) * fraction + start.x;
		y = (end.y - start.y) * fraction + start.y;
		z = (end.z - start.z) * fraction + start.z;

		return *this;
	}*/

	inline TVector3 add(const TVector3& rhs) const;
	inline void add(const TVector3& rhs, TVector3* const out_result) const;
	inline TVector3 add(const T rhs) const;
	inline TVector3& addLocal(const TVector3& rhs);
	inline TVector3& addLocal(const T rhs);
	inline TVector3& addLocal(const T rhsX, const T rhsY, const T rhsZ);

	inline TVector3 sub(const TVector3& rhs) const;
	inline TVector3 sub(const T rhs) const;
	inline void sub(const TVector3& var, TVector3* const out_result) const;
	inline TVector3& subLocal(const TVector3& rhs);
	inline TVector3& subLocal(const T rhs);

	inline TVector3 mul(const TVector3& rhs) const;
	inline void mul(const TVector3& rhs, TVector3* const out_result) const;
	inline TVector3 mul(const T rhs) const;
	inline void mul(const T rhs, TVector3* const out_result) const;
	inline TVector3& mulLocal(const T rhs);
	inline TVector3& mulLocal(const TVector3& rhs);

	inline TVector3 div(const TVector3& rhs) const;
	inline TVector3 div(const T rhs) const;
	inline TVector3& divLocal(const T rhs);
	inline TVector3& divLocal(const TVector3& rhs);

	inline TVector3& maddLocal(const T multiplier, const TVector3& adder);
	inline TVector3& negateLocal();

	inline TVector3 abs() const;
	inline TVector3& absLocal();

	// Current vector is expected to be normalized already.
	inline void calcOrthBasisAsYaxis(TVector3* const out_xAxis, TVector3* const out_zAxis) const;

	// Clamp current vector's components to specific range. If a component is NaN, its value is
	// clamped to lower bound. Either lower bound or upper bound shall not be NaN, or the method's 
	// behavior is undefined.
	inline TVector3 clamp(const T lowerBound, const T upperBound) const;
	inline TVector3& clampLocal(const T lowerBound, const T upperBound);

	inline TVector3 complement() const;
	inline real avg() const;
	inline TVector3 reciprocal() const;
	inline TVector3 reflect(const TVector3& normal) const;
	inline TVector3& reflectLocal(const TVector3& normal);

	// returned (x, y, z) = (min, mid, max)
	inline void sort(TVector3* const out_result) const;

	inline TVector3& set(const T rhsX, const T rhsY, const T rhsZ);
	inline TVector3& set(const T rhs);
	inline TVector3& set(const TVector3& rhs);
	inline void set(TVector3* const out_value) const;

	inline bool equals(const TVector3& rhs) const;
	inline bool equals(const TVector3& rhs, const T margin) const;
	inline bool allZero() const;

	inline std::string toStringFormal() const;
};

}// end namespace ph