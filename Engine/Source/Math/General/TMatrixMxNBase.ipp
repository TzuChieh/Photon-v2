#pragma once

#include "Math/General/TMatrixMxNBase.h"
#include "Common/assertion.h"

#include <utility>

namespace ph::math
{

template<typename Derived, typename T, std::size_t M, std::size_t N>
inline TMatrixMxNBase<Derived, T, M, N>::TMatrixMxNBase(const T elements)
{
	set(elements);
}

template<typename Derived, typename T, std::size_t M, std::size_t N>
inline TMatrixMxNBase<Derived, T, M, N>::TMatrixMxNBase(Elements elements) :
	m(std::move(elements))
{}

template<typename Derived, typename T, std::size_t M, std::size_t N>
inline constexpr std::size_t TMatrixMxNBase<Derived, T, M, N>::numRows() const noexcept
{
	return NUM_ROWS;
}

template<typename Derived, typename T, std::size_t M, std::size_t N>
inline constexpr std::size_t TMatrixMxNBase<Derived, T, M, N>::numCols() const noexcept
{
	return NUM_COLS;
}

template<typename Derived, typename T, std::size_t M, std::size_t N>
inline auto TMatrixMxNBase<Derived, T, M, N>::mul(const T constantValue) const
-> Derived
{
	return Derived(static_cast<const Derived&>(*this)).mulLocal(constantValue);
}

template<typename Derived, typename T, std::size_t M, std::size_t N>
inline auto TMatrixMxNBase<Derived, T, M, N>::mulLocal(const T constantValue)
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
inline auto TMatrixMxNBase<Derived, T, M, N>::multiplyVector(const RawColVecM& rhsColVector) const
-> RawColVecM
{
	RawColVecM col;
	for(std::size_t ri = 0; ri < M; ++ri)
	{
		T result = static_cast<T>(0);
		for(std::size_t ci = 0; ci < N; ++ci)
		{
			result += m[ri][ci] * rhsColVector[ri];
		}
		col[ri] = result;
	}
	return col;
}

template<typename Derived, typename T, std::size_t M, std::size_t N>
template<std::size_t K>
inline void TMatrixMxNBase<Derived, T, M, N>::multiplyMatrix(const TRawMatrixNxK<K>& rhsMatrix, TRawMatrixMxK<K>* const out_result) const
{
	PH_ASSERT(out_result);

	for(std::size_t mi = 0; mi < M; ++mi)
	{
		for(std::size_t ki = 0; ki < K; ++ki)
		{
			T result = static_cast<T>(0);
			for(std::size_t ni = 0; ni < N; ++ni)
			{
				result += m[mi][ni] * rhsMatrix[ni][ki];
			}
			(*out_result)[mi][ki] = result;
		}
	}
}

template<typename Derived, typename T, std::size_t M, std::size_t N>
template<std::size_t K>
inline void TMatrixMxNBase<Derived, T, M, N>::multiplyTransposedMatrix(const TRawMatrixKxN<K>& rhsMatrix, TRawMatrixMxK<K>* const out_result) const
{
	PH_ASSERT(out_result);

	for(std::size_t mi = 0; mi < M; ++mi)
	{
		for(std::size_t ki = 0; ki < K; ++ki)
		{
			T result = static_cast<T>(0);
			for(std::size_t ni = 0; ni < N; ++ni)
			{
				result += m[mi][ni] * rhsMatrix[ki][ni];
			}
			(*out_result)[mi][ki] = result;
		}
	}
}

template<typename Derived, typename T, std::size_t M, std::size_t N>
inline auto TMatrixMxNBase<Derived, T, M, N>::transposeMatrix() const
-> RawMatrixNxM
{
	RawMatrixNxM result;
	for(std::size_t ri = 0; ri < M; ++ri)
	{
		for(std::size_t ci = 0; ci < N; ++ci)
		{
			result[ci][ri] = m[ri][ci];
		}
	}
	return result;
}

template<typename Derived, typename T, std::size_t M, std::size_t N>
inline auto TMatrixMxNBase<Derived, T, M, N>::set(const T constantValue)
-> Derived&
{
	for(std::size_t ri = 0; ri < M; ++ri)
	{
		m[ri].fill(constantValue);
	}
	return static_cast<Derived&>(*this);
}

template<typename Derived, typename T, std::size_t M, std::size_t N>
inline auto TMatrixMxNBase<Derived, T, M, N>::operator [] (const std::size_t rowIndex)
-> RawRowVecN&
{
	PH_ASSERT_LT(rowIndex, M);

	return m[rowIndex];
}

template<typename Derived, typename T, std::size_t M, std::size_t N>
inline auto TMatrixMxNBase<Derived, T, M, N>::operator [] (const std::size_t rowIndex) const
-> const RawRowVecN&
{
	PH_ASSERT_LT(rowIndex, M);

	return m[rowIndex];
}

template<typename Derived, typename T, std::size_t M, std::size_t N>
inline std::string TMatrixMxNBase<Derived, T, M, N>::toString() const
{
	std::string result = "\n";
	for(std::size_t ri = 0; ri < M; ++ri)
	{
		result += "[";
		for(std::size_t ci = 0; ci < N; ++ci)
		{
			result += std::to_string(m[ri][ci]);
			result += (ci + 1 == N ? "" : ", ");
		}
		result += "]\n";
	}
	return result;
}

}// end namespace ph::math
