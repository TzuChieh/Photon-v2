#pragma once

#include "Math/General/TMatrixMxNBase.h"
#include "Utility/utility.h"

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
	PH_DEFINE_INLINE_RULE_OF_5_MEMBERS(TMatrixNBase);

public:
	/*! @brief Sets the matrix to be an identity matrix.
	*/
	Derived& setIdentity();

	Derived& setScale(const TRawColVector<T, N>& scaleFactor);

	Derived mul(const Derived& rhsMatrix) const;
	Derived& mulLocal(const Derived& rhsMatrix);
	Derived mulTransposed(const Derived& rhsMatrix) const;
	Derived& mulTransposedLocal(const Derived& rhsMatrix);
	void mul(const Derived& rhsMatrix, Derived* out_result) const;
	void mulTransposed(const Derived& rhsMatrix, Derived* out_result) const;
	Derived transpose() const;
	Derived& transposeLocal();

	using Base::Base;

	using Base::NUM_ROWS;
	using Base::NUM_COLS;
	using Base::numRows;
	using Base::numCols;

	using Base::mul;
	using Base::mulLocal;
	using Base::multiplyVector;
	using Base::multiplyMatrix;
	using Base::multiplyTransposedMatrix;
	using Base::transposeMatrix;
	using Base::set;

	using Base::operator [];

	using Base::toString;

	// TODO: inverse, determinant
};

}// end namespace ph::math

#include "Math/General/TMatrixNBase.ipp"
