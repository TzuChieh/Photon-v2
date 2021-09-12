#pragma once

#include "Common/primitive_type.h"
#include "Math/math_fwd.h"

#include <string>
#include <array>
#include <cstddef>
#include <type_traits>

namespace ph::math
{
	
/*! @brief A base for general N by M row-major matrices.

@tparam Derived Required to be the inheriting type.
@tparam T Type of elements in the matrix.
@tparam M Number of rows.
@tparam N Number of columns.
*/
template<typename Derived, typename T, std::size_t M, std::size_t N>
class TMatrixNxMBase
{
private:
	using Self = TMatrixNxMBase;

public:
	using Row = std::array<T, N>;
	using Col = std::array<T, M>;
	using Matrix = std::array<Row, M>;

	template<std::size_t K>
	using TMatrixNxK = std::array<std::array<T, K>, N>;

	template<std::size_t K>
	using TMatrixMxK = std::array<std::array<T, K>, M>;

	explicit TMatrixNxMBase(T elements);
	explicit TMatrixNxMBase(Matrix elements);

// Hide special members as this class is not intended to be used polymorphically.
// It is derived class's choice to expose them (by defining them in public) or not.
protected:
	inline TMatrixNxMBase() = default;
	inline TMatrixNxMBase(const TMatrixNxMBase& other) = default;
	inline TMatrixNxMBase(TMatrixNxMBase&& other) = default;
	inline TMatrixNxMBase& operator = (const TMatrixNxMBase& rhs) = default;
	inline TMatrixNxMBase& operator = (TMatrixNxMBase&& rhs) = default;
	inline ~TMatrixNxMBase() = default;

protected:
	inline static constexpr auto NUM_ROWS = M;
	inline static constexpr auto NUM_COLS = N;

	constexpr std::size_t numRows() const noexcept;
	constexpr std::size_t numCols() const noexcept;

	Derived mul(T constantValue) const;
	Col mul(const Row& rhsColVector) const;

	template<std::size_t K>
	Derived mul(const TMatrixNxK<K>& rhsMatrix) const;

	Derived& mulLocal(T constantValue);

	template<std::size_t K>
	Derived& mulLocal(const TMatrixNxK<K>& rhsMatrix) const;

	template<std::size_t K>
	void mul(const TMatrixNxK<K>& rhsMatrix, TMatrixMxK<K>* out_result) const;

	Derived& set(T constantValue);

	// TODO: inverse, determinant

	Row& operator [] (std::size_t rowIndex);
	const Row& operator [] (std::size_t rowIndex) const;

	std::string toString() const;

protected:
	Matrix m;
};

}// end namespace ph::math

#include "Math/General/TMatrixNxMBase.ipp"
