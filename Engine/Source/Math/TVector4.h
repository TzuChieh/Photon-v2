#pragma once

#include "Math/General/TVectorNBase.h"
#include "Math/math_fwd.h"
#include "Math/constant.h"

namespace ph::math
{

template<typename T>
class TVector4 final : public TVectorNBase<TVector4<T>, T, 4>
{
private:
	using Base = TVectorNBase<TVector4<T>, T, 4>;

protected:
	using Base::m;

public:
	inline TVector4() = default;
	inline TVector4(const TVector4& other) = default;
	inline TVector4(TVector4&& other) = default;
	inline TVector4& operator = (const TVector4& rhs) = default;
	inline TVector4& operator = (TVector4&& rhs) = default;
	inline ~TVector4() = default;

	using Base::Base;

	TVector4(T vx, T vy, T vz, T vw);

	template<typename U>
	explicit TVector4(const TVector4<U>& other);

	T& x();
	T& y();
	T& z();
	T& w();
	const T& x() const;
	const T& y() const;
	const T& z() const;
	const T& w() const;

	T& r();
	T& g();
	T& b();
	T& a();
	const T& r() const;
	const T& g() const;
	const T& b() const;
	const T& a() const;
};

}// end namespace ph::math

#include "Math/TVector4.ipp"
