#pragma once

#include "Common/primitive_type.h"
#include "Math/math_fwd.h"
#include "Utility/utility.h"

#include <string>
#include <array>
#include <cstddef>
#include <type_traits>

namespace ph::math
{
	
template<typename T, std::size_t N>
using TRawRowVector = std::array<T, N>;

template<typename T, std::size_t M>
using TRawColVector = std::array<T, M>;

template<typename T, std::size_t M, std::size_t N>
using TRawMatrix = std::array<std::array<T, N>, M>;

/*! @brief A base for general M by N row-major matrices.

@tparam Derived Required to be the inheriting type.
@tparam T Type of elements in the matrix.
@tparam M Number of rows.
@tparam N Number of columns.
*/
template<typename Derived, typename T, std::size_t M, std::size_t N>
class TMatrixMxNBase
{
private:
	using Self = TMatrixMxNBase;

public:
	using Elements = TRawMatrix<T, M, N>;

	explicit TMatrixMxNBase(T elements);
	explicit TMatrixMxNBase(Elements elements);

// Hide special members as this class is not intended to be used polymorphically.
// It is derived class's choice to expose them (by defining them in public) or not.
protected:
	PH_DEFINE_INLINE_RULE_OF_5_MEMBERS(TMatrixMxNBase);

protected:
	inline static constexpr auto NUM_ROWS = M;
	inline static constexpr auto NUM_COLS = N;

	constexpr std::size_t numRows() const noexcept;
	constexpr std::size_t numCols() const noexcept;

	Derived mul(T constantValue) const;
	Derived& mulLocal(T constantValue);

	Derived& set(T constantValue);

	TRawColVector<T, M> multiplyVector(const TRawColVector<T, M>& rhsColVector) const;

	template<std::size_t K>
	void multiplyMatrix(const TRawMatrix<T, N, K>& rhsMatrix, TRawMatrix<T, M, K>* out_result) const;

	template<std::size_t K>
	void multiplyTransposedMatrix(const TRawMatrix<T, K, N>& rhsMatrix, TRawMatrix<T, M, K>* out_result) const;

	TRawMatrix<T, N, M> transposeMatrix() const;

	TRawRowVector<T, N>& operator [] (std::size_t rowIndex);
	const TRawRowVector<T, N>& operator [] (std::size_t rowIndex) const;

	std::string toString() const;

protected:
	TRawMatrix<T, M, N> m;
};

}// end namespace ph::math

#include "Math/General/TMatrixMxNBase.ipp"
