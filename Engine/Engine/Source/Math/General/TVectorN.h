#pragma once

#include "Math/General/TVectorNBase.h"

#include <cstddef>

namespace ph::math
{

template<typename T, std::size_t N>
class TVectorN final : public TVectorNBase<TVectorN<T, N>, T, N>
{
private:
	using Base = TVectorNBase<TVectorN<T, N>, T, N>;

protected:
	using Base::m;

public:
	inline TVectorN() = default;
	inline TVectorN(const TVectorN& other) = default;
	inline TVectorN(TVectorN&& other) = default;
	inline TVectorN& operator = (const TVectorN& rhs) = default;
	inline TVectorN& operator = (TVectorN&& rhs) = default;
	inline ~TVectorN() = default;

	using Base::Base;

	template<typename U>
	explicit TVectorN(const TVectorN<U, N>& other);
};

}// end namespace ph::math

#include "Math/General/TVectorN.ipp"
