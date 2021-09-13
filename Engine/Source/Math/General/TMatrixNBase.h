#pragma once

#include "Math/General/TMatrixMxNBase.h"

namespace ph::math
{

/*! @brief A base for general N dimensional row-major square matrices.

@tparam Derived Required to be the inheriting type.
@tparam T Type of elements in the matrix.
@tparam N Number of rows and columns.
*/
template<typename Derived, typename T, std::size_t N>
class TMatrixNBase : public TMatrixMxNBase<Derived, T, N, N>
{
private:
	using Base = TMatrixMxNBase<Derived, T, N, N>;
	using Self = TMatrixNBase;

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
	/*! @brief Sets the matrix to be an identity matrix.
	*/
	Derived& setIdentity();

	Derived mul(const Derived& rhsMatrix) const;
	Derived& mulLocal(const Derived& rhsMatrix);
	void mul(const Derived& rhsMatrix, Derived* out_result) const;

	using Base::Base;

	using Base::NUM_ROWS;
	using Base::NUM_COLS;
	using Base::numRows;
	using Base::numCols;

	using Base::mul;
	using Base::mulLocal;

	using Base::operator [];

	using Base::toString;

	// TODO: inverse, determinant
};

}// end namespace ph::math

#include "Math/General/TMatrixNBase.ipp"
