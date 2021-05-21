#pragma once

#include "Common/primitive_type.h"
#include "Math/math_fwd.h"
#include "Math/constant.h"

#include <cmath>
#include <string>
#include <cstdlib>
#include <array>

namespace ph::math
{

template<typename T>
class TVector3 final
{
public:
	static TVector3 weightedSum(
		const TVector3& vA, T wA, 
		const TVector3& vB, T wB, 
		const TVector3& vC, T wC);

	static TVector3 lerp(
		const TVector3& vA, 
		const TVector3& vB, 
		T               parametricT);

public:
	T x;
	T y;
	T z;

public:
	inline TVector3() = default;
	TVector3(T x, T y, T z);
	explicit TVector3(T value);
	inline TVector3(const TVector3& other) = default;

	template<typename U>
	explicit TVector3(const TVector3<U>& other);

	template<typename U>
	explicit TVector3(const std::array<U, 3>& xyzValues);

	template<typename U>
	explicit TVector3(const TArithmeticArray<U, 3>& xyzValues);

	TVector3 rotate(const TQuaternion<T>& rotation) const;
	void rotate(const TQuaternion<T>& rotation, TVector3* out_result) const;

	T length() const;
	T lengthSquared() const;

	T max() const;
	T absMax() const;
	TVector3 max(const TVector3& rhs) const;
	TVector3& maxLocal(const TVector3& rhs);
	TVector3 min(const TVector3& rhs) const;
	TVector3& minLocal(const TVector3& rhs);
	constant::AxisIndexType maxDimension() const;
	TVector3 abs() const;
	TVector3& absLocal();

	T dot(const TVector3& rhs) const;
	T dot(T rhs) const;
	T dot(const T rhsX, const T rhsY, const T rhsZ) const;
	T absDot(const TVector3& rhs) const;

	TVector3 cross(const TVector3& rhs) const;
	void cross(const TVector3& rhs, TVector3* out_result) const;

	// Notice that normalizing a integer typed vector is undefined behavior.
	TVector3 normalize() const;
	TVector3& normalizeLocal();

	TVector3 add(const TVector3& rhs) const;
	void add(const TVector3& rhs, TVector3* out_result) const;
	TVector3 add(T rhs) const;
	TVector3& addLocal(const TVector3& rhs);
	TVector3& addLocal(T rhs);
	TVector3& addLocal(const T rhsX, const T rhsY, const T rhsZ);

	TVector3 sub(const TVector3& rhs) const;
	TVector3 sub(T rhs) const;
	void sub(const TVector3& var, TVector3* out_result) const;
	TVector3& subLocal(const TVector3& rhs);
	TVector3& subLocal(T rhs);

	TVector3 mul(const TVector3& rhs) const;
	void mul(const TVector3& rhs, TVector3* out_result) const;
	TVector3 mul(T rhs) const;
	void mul(T rhs, TVector3* out_result) const;
	TVector3& mulLocal(T rhs);
	TVector3& mulLocal(T rhsX, T rhsY, T rhsZ);
	TVector3& mulLocal(const TVector3& rhs);
	
	TVector3 div(const TVector3& rhs) const;
	TVector3& divLocal(const TVector3& rhs);
	TVector3 div(T rhs) const;
	TVector3& divLocal(T rhs);
	
	TVector3 complement() const;
	TVector3 negate() const;
	TVector3& negateLocal();
	TVector3 reciprocal() const;
	TVector3& maddLocal(T multiplier, const TVector3& adder);
	T avg() const;
	TVector3 reflect(const TVector3& normal) const;
	TVector3& reflectLocal(const TVector3& normal);

	// TODO: supply clamp methods for NaN-safe and NaN-propagative versions

	// Clamp current vector's components to specific range. If a component 
	// is NaN, its value is clamped to lower bound. Either lower bound or 
	// upper bound shall not be NaN, or the method's behavior is undefined.
	TVector3 clamp(T lowerBound, T upperBound) const;
	TVector3& clampLocal(T lowerBound, T upperBound);

	// returned (x, y, z) = (min, mid, max)
	void sort(TVector3* out_result) const;

	TVector3& set(T rhsX, T rhsY, T rhsZ);
	TVector3& set(T rhs);
	TVector3& set(const TVector3& rhs);
	void set(TVector3* const out_value) const;
	TVector3& set(constant::AxisIndexType axisIndex, T value);

	bool equals(const TVector3& other) const;// TODO: rename to isEqual
	bool equals(const TVector3& other, T margin) const;// TODO: rename to isNear
	bool isZero() const;
	bool isNotZero() const;
	bool hasNegativeComponent() const;
	bool isFinite() const;

	std::string toString() const;

	T& operator [] (constant::AxisIndexType axisIndex);
	const T& operator [] (constant::AxisIndexType axisIndex) const;

	TVector3 operator * (T rhs) const;
	TVector3 operator / (T rhs) const;
	TVector3 operator + (T rhs) const;
	TVector3 operator + (const TVector3& rhs) const;
	TVector3 operator - (T rhs) const;
	TVector3 operator - (const TVector3& rhs) const;

	bool operator == (const TVector3& rhs) const;
	bool operator != (const TVector3& rhs) const;
};

template<typename T>
inline TVector3<T> operator * (T rhs, const TVector3<T>& lhs);

}// end namespace ph::math

#include "Math/TVector3.ipp"
