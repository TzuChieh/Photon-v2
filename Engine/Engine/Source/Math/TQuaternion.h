#pragma once

#include "Math/General/TArithmeticArrayBase.h"
#include "Math/math_fwd.h"
#include "Utility/utility.h"

#include <array>
#include <cmath>

namespace ph::math
{

/*! @brief Represents a quaternion.
*/
template<typename T>
class TQuaternion final : public TArithmeticArrayBase<TQuaternion<T>, T, 4>
{
private:
	using Base = TArithmeticArrayBase<TQuaternion<T>, T, 4>;

protected:
	using Base::m;

public:
	static TQuaternion makeNoRotation();

public:
	PH_DEFINE_INLINE_RULE_OF_5_MEMBERS(TQuaternion);

	using Base::Base;

	TQuaternion(T vx, T vy, T vz, T vw);

	template<typename U>
	explicit TQuaternion(const TQuaternion<U>& other);

	template<typename U>
	explicit TQuaternion(const std::array<U, 4>& xyzwValues);

	TQuaternion(const TVector3<T>& normalizedAxis, T radians);
	explicit TQuaternion(const TMatrix4<T>& rotationMatrix);

	T& x();
	T& y();
	T& z();
	T& w();
	const T& x() const;
	const T& y() const;
	const T& z() const;
	const T& w() const;

	/*! @brief Quaternion multiplication (treating the input's w component as 0).
	*/
	TQuaternion mul(const TVector3<T>& xyz) const;

	TQuaternion normalize() const;
	TQuaternion& normalizeLocal();
	T length() const;

	TQuaternion conjugate() const;
	void conjugate(TQuaternion* out_result) const;
	TQuaternion& conjugateLocal();

	TQuaternion mul(const TQuaternion& rhs) const;
	TQuaternion& mulLocal(const TQuaternion& rhs);
	TQuaternion mul(T rhs) const;

	TQuaternion sub(const TQuaternion& rhs) const;
	TQuaternion add(const TQuaternion& rhs) const;
	T dot(const TQuaternion& rhs) const;

	void setRot(const TVector3<T>& normalizedAxis, T radians);
	void toRotationMatrix(TMatrix4<T>* out_result) const;

	using Base::set;
	using Base::begin;
	using Base::end;

	using Base::isEqual;

	using Base::toString;
	using Base::toVector;
	using Base::toArray;
	using Base::toSpan;
	using Base::toView;

	using Base::operator [];
	using Base::operator ==;
	using Base::operator !=;
};

}// end namespace ph::math

#include "Math/TQuaternion.ipp"
