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
	using RawMatrixMxN = std::array<std::array<T, N>, M>;
	using Elements     = RawMatrixMxN;

	explicit TMatrixMxNBase(T elements);
	explicit TMatrixMxNBase(Elements elements);

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
	using RawRowVecN = std::array<T, N>;
	using RawColVecM = std::array<T, M>;

	template<std::size_t K>
	using TRawMatrixNxK = std::array<std::array<T, K>, N>;

	template<std::size_t K>
	using TRawMatrixMxK = std::array<std::array<T, K>, M>;

	template<std::size_t K>
	using TRawMatrixKxN = std::array<std::array<T, N>, K>;

	using RawMatrixNxM = std::array<std::array<T, M>, N>;

	inline static constexpr auto NUM_ROWS = M;
	inline static constexpr auto NUM_COLS = N;

	constexpr std::size_t numRows() const noexcept;
	constexpr std::size_t numCols() const noexcept;

	Derived mul(T constantValue) const;
	Derived& mulLocal(T constantValue);

	Derived& set(T constantValue);

	RawColVecM multiplyVector(const RawColVecM& rhsColVector) const;

	template<std::size_t K>
	void multiplyMatrix(const TRawMatrixNxK<K>& rhsMatrix, TRawMatrixMxK<K>* out_result) const;

	template<std::size_t K>
	void multiplyTransposedMatrix(const TRawMatrixKxN<K>& rhsMatrix, TRawMatrixMxK<K>* out_result) const;

	RawMatrixNxM transposeMatrix() const;

	RawRowVecN& operator [] (std::size_t rowIndex);
	const RawRowVecN& operator [] (std::size_t rowIndex) const;

	std::string toString() const;

protected:
	RawMatrixMxN m;
};

}// end namespace ph::math

#include "Math/General/TMatrixMxNBase.ipp"
