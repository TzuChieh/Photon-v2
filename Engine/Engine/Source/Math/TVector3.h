#pragma once

#include "Math/General/TVectorNBase.h"
#include "Math/math_fwd.h"
#include "Math/constant.h"
#include "Utility/utility.h"

#include <Utility/string_utils.h>

namespace ph::math
{

template<typename T>
class TVector3 final : public TVectorNBase<TVector3<T>, T, 3>
{
private:
	using Base = TVectorNBase<TVector3<T>, T, 3>;

protected:
	using Base::m;

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
	PH_DEFINE_INLINE_RULE_OF_5_MEMBERS(TVector3);

	using Base::Base;

	TVector3(T vx, T vy, T vz);

	template<typename U>
	explicit TVector3(const TVector3<U>& other);

	TVector3 rotate(const TQuaternion<T>& rotation) const;
	void rotate(const TQuaternion<T>& rotation, TVector3* out_result) const;

	TVector3 cross(const TVector3& rhs) const;
	void cross(const TVector3& rhs, TVector3* out_result) const;

	TVector3& maddLocal(T multiplier, const TVector3& adder);
	
	TVector3 reflect(const TVector3& normal) const;
	TVector3& reflectLocal(const TVector3& normal);

	// returned (x, y, z) = (min, mid, max)
	void sort(TVector3* out_result) const;

	T& x();
	T& y();
	T& z();
	const T& x() const;
	const T& y() const;
	const T& z() const;

	T& r();
	T& g();
	T& b();
	const T& r() const;
	const T& g() const;
	const T& b() const;
};

template<typename T>
inline TVector3<T> operator * (T rhs, const TVector3<T>& lhs);

}// end namespace ph::math

template<typename T>
PH_DEFINE_INLINE_TO_STRING_FORMATTER_TEMPLATE(ph::math::TVector3<T>);

#include "Math/TVector3.ipp"
