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
	static inline TVector3 weightedSum(const TVector3& vA, T wA, 
	                                   const TVector3& vB, T wB, 
	                                   const TVector3& vC, T wC);

	static TVector3 lerp(const TVector3& vA, const TVector3& vB, T parametricT);

public:
	T x;
	T y;
	T z;

public:
	inline TVector3();
	inline TVector3(T x, T y, T z);
	inline explicit TVector3(T value);
	inline TVector3(const TVector3& other);

	template<typename U>
	inline explicit TVector3(const TVector3<U>& other);

	virtual inline ~TVector3() = default;

	TVector3 rotate(const TQuaternion<T>& rotation) const;
	void rotate(const TQuaternion<T>& rotation, TVector3* out_result) const;

	inline T length() const;
	inline T lengthSquared() const;

	inline T max() const;
	inline T absMax() const;
	inline TVector3 max(const TVector3& rhs) const;
	inline TVector3& maxLocal(const TVector3& rhs);
	inline TVector3 min(const TVector3& rhs) const;
	inline TVector3& minLocal(const TVector3& rhs);
	inline int32 maxDimension() const;
	inline TVector3 abs() const;
	inline TVector3& absLocal();

	inline T dot(const TVector3& rhs) const;
	inline T dot(T rhs) const;
	inline T dot(const T rhsX, const T rhsY, const T rhsZ) const;
	inline T absDot(const TVector3& rhs) const;

	inline TVector3 cross(const TVector3& rhs) const;
	inline void cross(const TVector3& rhs, TVector3* out_result) const;

	// Notice that normalizing a integer typed vector is undefined behavior.
	inline TVector3 normalize() const;
	inline TVector3& normalizeLocal();

	inline TVector3 add(const TVector3& rhs) const;
	inline void add(const TVector3& rhs, TVector3* out_result) const;
	inline TVector3 add(T rhs) const;
	inline TVector3& addLocal(const TVector3& rhs);
	inline TVector3& addLocal(T rhs);
	inline TVector3& addLocal(const T rhsX, const T rhsY, const T rhsZ);

	inline TVector3 sub(const TVector3& rhs) const;
	inline TVector3 sub(T rhs) const;
	inline void sub(const TVector3& var, TVector3* out_result) const;
	inline TVector3& subLocal(const TVector3& rhs);
	inline TVector3& subLocal(T rhs);

	inline TVector3 mul(const TVector3& rhs) const;
	inline void mul(const TVector3& rhs, TVector3* out_result) const;
	inline TVector3 mul(T rhs) const;
	inline void mul(T rhs, TVector3* out_result) const;
	inline TVector3& mulLocal(T rhs);
	inline TVector3& mulLocal(T rhsX, T rhsY, T rhsZ);
	inline TVector3& mulLocal(const TVector3& rhs);
	
	inline TVector3 div(const TVector3& rhs) const;
	inline TVector3& divLocal(const TVector3& rhs);
	inline TVector3 div(T rhs) const;
	inline TVector3& divLocal(T rhs);
	
	inline TVector3 complement() const;
	inline TVector3& negateLocal();
	inline TVector3 reciprocal() const;
	inline TVector3& maddLocal(T multiplier, const TVector3& adder);
	inline T avg() const;
	inline TVector3 reflect(const TVector3& normal) const;
	inline TVector3& reflectLocal(const TVector3& normal);

	// TODO: supply clamp methods for NaN-safe and NaN-propagative versions

	// Clamp current vector's components to specific range. If a component 
	// is NaN, its value is clamped to lower bound. Either lower bound or 
	// upper bound shall not be NaN, or the method's behavior is undefined.
	inline TVector3 clamp(T lowerBound, T upperBound) const;
	inline TVector3& clampLocal(T lowerBound, T upperBound);

	// returned (x, y, z) = (min, mid, max)
	inline void sort(TVector3* out_result) const;

	inline TVector3& set(T rhsX, T rhsY, T rhsZ);
	inline TVector3& set(T rhs);
	inline TVector3& set(const TVector3& rhs);
	inline void set(TVector3* const out_value) const;

	inline bool equals(const TVector3& other) const;
	inline bool equals(const TVector3& other, T margin) const;
	inline bool isZero() const;
	inline bool isNotZero() const;
	inline bool hasNegativeComponent() const;
	inline bool isRational() const;

	inline std::string toString() const;

	inline T& operator [] (int32 axisIndex);
	inline const T& operator [] (int32 axisIndex) const;

	inline TVector3 operator * (T rhs) const;
	inline TVector3 operator / (T rhs) const;
	inline TVector3 operator + (T rhs) const;
	inline TVector3 operator + (const TVector3& rhs) const;
	inline TVector3 operator - (T rhs) const;
	inline TVector3 operator - (const TVector3& rhs) const;

	inline TVector3& operator = (const TVector3& rhs);
};

template<typename T>
inline TVector3<T> operator * (T rhs, const TVector3<T>& lhs);

}// end namespace ph

#include "Math/TVector3.ipp"