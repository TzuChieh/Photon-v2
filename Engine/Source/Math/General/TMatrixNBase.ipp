#pragma once

#include "Math/General/TMatrixNBase.h"
#include "Common/assertion.h"

namespace ph::math
{

template<typename Derived, typename T, std::size_t N>
inline auto TMatrixNBase<Derived, T, N>::setIdentity()
-> Derived&
{
	set(static_cast<T>(0));

	for(std::size_t ni = 0; ni < N; ++ni)
	{
		m[ni][ni] = static_cast<T>(1);
	}

	return static_cast<Derived&>(*this);
}

template<typename Derived, typename T, std::size_t N>
inline auto TMatrixNBase<Derived, T, N>::mul(const Derived& rhsMatrix) const
-> Derived
{
	Derived result(static_cast<const Derived&>(*this));
	mul(rhsMatrix, &result);
	return result;
}

template<typename Derived, typename T, std::size_t N>
inline auto TMatrixNBase<Derived, T, N>::mulLocal(const Derived& rhsMatrix)
-> Derived&
{
	Derived result(static_cast<const Derived&>(*this));
	mul(rhsMatrix, &result);
	Derived::operator = (result);

	return static_cast<Derived&>(*this);
}

template<typename Derived, typename T, std::size_t N>
inline void TMatrixNBase<Derived, T, N>::mul(const Derived& rhsMatrix, Derived* const out_result) const
{
	PH_ASSERT(out_result);
	PH_ASSERT(out_result != this);

	Base::template multiplyMatrix<N>(rhsMatrix.Self::m, &(out_result->Self::m));
}

}// end namespace ph::math
