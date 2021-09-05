#pragma once

#include "Math/General/TVectorNBase.h"
#include "Math/math_fwd.h"
#include "Math/constant.h"

namespace ph::math
{

template<typename T>
class TVector2 final : private TVectorNBase<TVector2<T>, T, 2>
{
private:
	using Base = TVectorNBase<TVector2<T>, T, 2>;

protected:
	using Base::m;

public:
	inline TVector2() = default;
	inline TVector2(const TVector2& other) = default;
	inline TVector2(TVector2&& other) = default;
	inline TVector2& operator = (const TVector2& rhs) = default;
	inline TVector2& operator = (TVector2&& rhs) = default;

	using Base::Base;

	TVector2(T vx, T vy);

	template<typename U>
	explicit TVector2(const TVector2<U>& other);

	T& x();
	T& y();
	const T& x() const;
	const T& y() const;
};

}// end namespace ph::math

#include "Math/TVector2.ipp"
