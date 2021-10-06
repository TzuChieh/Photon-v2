#pragma once

#include "Math/General/TMatrixNBase.h"
#include "Math/math_fwd.h"

#include <string>
#include <array>

namespace ph::math
{

template<typename T>
class TMatrix3 final : public TMatrixNBase<TMatrix3<T>, T, 3>
{
private:
	using Base = TMatrixNBase<TMatrix3<T>, T, 3>;

protected:
	using Base::m;

public:
	inline TMatrix3() = default;
	inline TMatrix3(const TMatrix3& other) = default;
	inline TMatrix3(TMatrix3&& other) = default;
	inline TMatrix3& operator = (const TMatrix3& rhs) = default;
	inline TMatrix3& operator = (TMatrix3&& rhs) = default;
	inline ~TMatrix3() = default;

	using Base::Base;

	template<typename U>
	explicit TMatrix3(const TMatrix3<U>& other);

	TMatrix3(
		T m00, T m01, T m02,
		T m10, T m11, T m12,
		T m20, T m21, T m22);

public:
	TVector3<T> mul(const TVector3<T>& rhsColVector) const;

	TMatrix3 inverse() const;
	T determinant() const;

	TVector3<T> operator * (const TVector3<T>& rhsColVector) const;

	using Base::mul;
};

}// end namespace ph::math

#include "Math/TMatrix3.ipp"
