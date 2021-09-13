#pragma once

#include "Common/primitive_type.h"
#include "Math/math_fwd.h"

#include <string>
#include <array>
#include <cstddef>
#include <type_traits>

namespace ph::math
{
	
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
	using MatrixMxN = std::array<std::array<T, N>, M>;
	using Elements  = MatrixMxN;

	explicit TMatrixMxNBase(T elements);
	explicit TMatrixMxNBase(MatrixMxN elements);

// Hide special members as this class is not intended to be used polymorphically.
// It is derived class's choice to expose them (by defining them in public) or not.
protected:
	inline TMatrixMxNBase() = default;
	inline TMatrixMxNBase(const TMatrixMxNBase& other) = default;
	inline TMatrixMxNBase(TMatrixMxNBase&& other) = default;
	inline TMatrixMxNBase& operator = (const TMatrixMxNBase& rhs) = default;
	inline TMatrixMxNBase& operator = (TMatrixMxNBase&& rhs) = default;
	inline ~TMatrixMxNBase() = default;

protected:
	using RowVecN = std::array<T, N>;
	using ColVecM = std::array<T, M>;

	template<std::size_t K>
	using TMatrixNxK = std::array<std::array<T, K>, N>;

	template<std::size_t K>
	using TMatrixMxK = std::array<std::array<T, K>, M>;

	template<std::size_t K>
	using TMatrixKxN = std::array<std::array<T, N>, K>;

	using MatrixNxM = std::array<std::array<T, M>, N>;

	inline static constexpr auto NUM_ROWS = M;
	inline static constexpr auto NUM_COLS = N;

	constexpr std::size_t numRows() const noexcept;
	constexpr std::size_t numCols() const noexcept;

	Derived mul(T constantValue) const;
	Derived& mulLocal(T constantValue);

	Derived& set(T constantValue);

	ColVecM multiplyVector(const RowVecN& rhsColVector) const;

	template<std::size_t K>
	void multiplyMatrix(const TMatrixNxK<K>& rhsMatrix, TMatrixMxK<K>* out_result) const;

	template<std::size_t K>
	void multiplyTransposedMatrix(const TMatrixKxN<K>& rhsMatrix, TMatrixMxK<K>* out_result) const;

	MatrixNxM transposeMatrix() const;

	RowVecN& operator [] (std::size_t rowIndex);
	const RowVecN& operator [] (std::size_t rowIndex) const;

	std::string toString() const;

protected:
	MatrixMxN m;
};

}// end namespace ph::math

#include "Math/General/TMatrixMxNBase.ipp"
