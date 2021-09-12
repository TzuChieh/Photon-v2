#pragma once

#include "Math/General/TMatrixNxMBase.h"

namespace ph::math
{

/*! @brief A base for general N dimensional row-major square matrices.

@tparam Derived Required to be the inheriting type.
@tparam T Type of elements in the matrix.
@tparam N Number of rows and columns.
*/
template<typename Derived, typename T, std::size_t N>
class TMatrixNBase : private TMatrixNxMBase<Derived, T, N, N>
{
private:
	using Base = TMatrixNxMBase<Derived, T, N, N>;

protected:
	using Base::m;

// Hide special members as this class is not intended to be used polymorphically.
// It is derived class's choice to expose them (by defining them in public) or not.
protected:
	inline TMatrixNBase() = default;
	inline TMatrixNBase(const TMatrixNBase& other) = default;
	inline TMatrixNBase(TMatrixNBase&& other) = default;
	inline TMatrixNBase& operator = (const TMatrixNBase& rhs) = default;
	inline TMatrixNBase& operator = (TMatrixNBase&& rhs) = default;
	inline ~TMatrixNBase() = default;

public:
	/*! @brief Returns an identity matrix. This method is defined only for square matrices.
	*/
	template<typename = std::enable_if_t<(M == N)>>
	static Derived makeIdentity();

	Derived& initIdentity();

	using Base::NUM_ROWS;
	using Base::NUM_COLS;
	using Base::numRows;
	using Base::numCols;

	using Base::mul;
	using Base::mulLocal;

	using Base::operator [];

	using Base::toString;
};

}// end namespace ph::math

#include "Math/General/TMatrixNBase.ipp"
