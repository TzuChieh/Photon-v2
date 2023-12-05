#pragma once

#include "Math/General/TVectorNBase.h"
#include "Math/math_fwd.h"
#include "Math/constant.h"
#include "Math/hash.h"

#include <Utility/string_utils.h>

#include <functional>

namespace ph::math
{

template<typename T>
class TVector2 final : public TVectorNBase<TVector2<T>, T, 2>
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
	inline ~TVector2() = default;

	using Base::Base;

	TVector2(T vx, T vy);

	template<typename U>
	explicit TVector2(const TVector2<U>& other);

	template<typename U>
	TVector2<U> losslessCast() const;

	T& x();
	T& y();
	const T& x() const;
	const T& y() const;

	T& u();
	T& v();
	const T& u() const;
	const T& v() const;
};

}// end namespace ph::math

template<typename T>
PH_DEFINE_INLINE_TO_STRING_FORMATTER_TEMPLATE(ph::math::TVector2<T>);

namespace std
{

template<typename T>
struct hash<ph::math::TVector2<T>>
{
	std::size_t operator () (const ph::math::TVector2<T>& vec2) const
	{
		return ph::math::murmur3_32(vec2, 0);
	}
};

}// end namespace std

#include "Math/TVector2.ipp"
