#pragma once

#include "Math/General/TMatrixNBase.h"

#include <Common/assertion.h>

#include <utility>

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
inline auto TMatrixNBase<Derived, T, N>::setScale(const TRawColVector<T, N>& scaleFactor)
-> Derived&
{
	set(static_cast<T>(0));

	for(std::size_t ni = 0; ni < N; ++ni)
	{
		m[ni][ni] = scaleFactor[ni];
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
inline auto TMatrixNBase<Derived, T, N>::mulTransposed(const Derived& rhsMatrix) const
-> Derived
{
	Derived result(static_cast<const Derived&>(*this));
	mulTransposed(rhsMatrix, &result);
	return result;
}

template<typename Derived, typename T, std::size_t N>
inline auto TMatrixNBase<Derived, T, N>::mulTransposedLocal(const Derived& rhsMatrix)
-> Derived&
{
	Derived result(static_cast<const Derived&>(*this));
	mulTransposed(rhsMatrix, &result);
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

template<typename Derived, typename T, std::size_t N>
inline void TMatrixNBase<Derived, T, N>::mulTransposed(const Derived& rhsMatrix, Derived* const out_result) const
{
	PH_ASSERT(out_result);

	Base::template multiplyTransposedMatrix<N>(rhsMatrix.Self::m, &(out_result->Self::m));
}

template<typename Derived, typename T, std::size_t N>
inline auto TMatrixNBase<Derived, T, N>::transpose() const
-> Derived
{
	Derived result(static_cast<const Derived&>(*this));
	result.Self::transposeLocal();
	return result;
}

template<typename Derived, typename T, std::size_t N>
inline auto TMatrixNBase<Derived, T, N>::transposeLocal()
-> Derived&
{
	// We did not use the general transpose from base here--as square matrices
	// can be efficiently transposed locally.

	static_assert(N >= 1);

	// Skip N = 1 as it is already a transpose by itself
	for(std::size_t ri = 0; ri < N - 1; ++ri)
	{
		// Skip ci = ri as diagonal elements do not change during a transpose
		for(std::size_t ci = ri + 1; ci < N; ++ci)
		{
			// Enable ADL
			using std::swap;

			swap(m[ri][ci], m[ci][ri]);
		}
	}

	return static_cast<Derived&>(*this);
}

}// end namespace ph::math
