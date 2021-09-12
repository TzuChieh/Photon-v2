#pragma once

#include "Math/General/TMatrixNxMBase.h"
#include "Common/assertion.h"

#include <utility>

namespace ph::math
{

template<typename Derived, typename T, std::size_t M, std::size_t N>
inline TMatrixNxMBase<Derived, T, M, N>::TMatrixNxMBase(const T elements)
{
	set(elements);
}

template<typename Derived, typename T, std::size_t M, std::size_t N>
inline TMatrixNxMBase<Derived, T, M, N>::TMatrixNxMBase(Matrix elements) :
	m(std::move(elements))
{}

template<typename Derived, typename T, std::size_t M, std::size_t N>
inline constexpr std::size_t TMatrixNxMBase<Derived, T, M, N>::numRows() const noexcept
{
	return NUM_ROWS;
}

template<typename Derived, typename T, std::size_t M, std::size_t N>
inline constexpr std::size_t TMatrixNxMBase<Derived, T, M, N>::numCols() const noexcept
{
	return NUM_COLS;
}

template<typename Derived, typename T, std::size_t M, std::size_t N>
inline auto TMatrixNxMBase<Derived, T, M, N>::mul(const T constantValue) const
-> Derived
{
	return Derived(static_cast<const Derived&>(*this)).mulLocal(constantValue);
}

template<typename Derived, typename T, std::size_t M, std::size_t N>
inline auto TMatrixNxMBase<Derived, T, M, N>::mulLocal(const T constantValue)
-> Derived&
{
	for(std::size_t ri = 0; ri < M; ++ri)
	{
		for(std::size_t ci = 0; ci < N; ++ci)
		{
			m[ri][ci] *= constantValue;
		}
	}
	return static_cast<Derived&>(*this);
}

template<typename Derived, typename T, std::size_t M, std::size_t N>
inline auto TMatrixNxMBase<Derived, T, M, N>::mul(const Row& rhsColVector) const
-> Col
{
	Col col;
	for(std::size_t ri = 0; ri < M; ++ri)
	{
		col[ri] = static_cast<T>(0);
		for(std::size_t ci = 0; ci < N; ++ci)
		{
			col[ri] += m[ri][ci] * rhsColVector[ci];
		}
	}
	return col;
}

template<typename Derived, typename T, std::size_t M, std::size_t N>
inline auto TMatrixNxMBase<Derived, T, M, N>::mulLocal(const Derived& rhsMatrix) const
-> Derived&
{
	for(std::size_t ri = 0; ri < M; ++ri)
	{
		for(std::size_t ci = 0; ci < N; ++ci)
		{
			col[ri] += m[ri][ci] * rhsColVector[ci];
		}
	}
	return static_cast<Derived&>(*this);
}

template<typename Derived, typename T, std::size_t M, std::size_t N>
template<std::size_t K>
inline void TMatrixNxMBase<Derived, T, M, N>::mul(const TMultipliableRhsMatrix<K>& rhsMatrix, Derived* const out_result) const
{
	PH_ASSERT(out_result);
	PH_ASSERT(this != static_cast<const Self*>(out_result));

	for(std::size_t mi = 0; mi < M; ++mi)
	{
		for(std::size_t ni = 0; ni < N; ++ni)
		{
			out_result->Self::m[mi][ni] = static_cast<T>(0);
			for(std::size_t ki = 0; ki < K; ++ki)
			{

			}
		}
	}
}

template<typename Derived, typename T, std::size_t M, std::size_t N>
inline auto TMatrixNxMBase<Derived, T, M, N>::set(const T constantValue)
-> Derived&
{
	for(std::size_t ri = 0; ri < M; ++ri)
	{
		m[ri].fill(constantValue);
	}
	return static_cast<Derived&>(*this);
}

template<typename Derived, typename T, std::size_t M, std::size_t N>
inline auto TMatrixNxMBase<Derived, T, M, N>::operator [] (const std::size_t rowIndex)
-> Row&
{
	PH_ASSERT_LT(rowIndex, M);

	return m[rowIndex];
}

template<typename Derived, typename T, std::size_t M, std::size_t N>
inline auto TMatrixNxMBase<Derived, T, M, N>::operator [] (const std::size_t rowIndex) const
-> const Row&
{
	PH_ASSERT_LT(rowIndex, M);

	return m[rowIndex];
}

}// end namespace ph::math
