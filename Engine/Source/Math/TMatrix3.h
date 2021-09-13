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

	using Base::Base;

	template<typename U>
	explicit TMatrix3(const TMatrix3<U>& other);

	TMatrix3 inverse() const;
	T determinant() const;
};

}// end namespace ph::math

#include "Math/TMatrix3.ipp"
