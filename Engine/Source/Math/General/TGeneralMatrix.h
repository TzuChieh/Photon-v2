#pragma once

#include "Common/primitive_type.h"
#include "Math/math_fwd.h"

#include <string>
#include <array>
#include <cstddef>
#include <type_traits>

namespace ph
{

// WARNING: this class is unimplemented
	
/*
	A base for general matrices. The meanings and constraints on the template
	parameters are as follows:

	<Matrix>: required to be the inheriting type
	<T>:      type of elements in the matrix
	<M>:      number of rows
	<N>:      number of columns
*/
template<typename Matrix, typename T, std::size_t M, std::size_t N>
class TGeneralMatrix
{
public:
	typedef std::array<std::array<T, N>, M> Elements;

	template<std::size_t K>
	using RightMultipliableMatrix = TAbstractMatrix<Matrix, T, N, K>;

	template<std::size_t K>
	using RightMultipliedMatrix = TAbstractMatrix<Matrix, T, N, K>;

	Elements m;

public:
	// Constructs an zero-initialized matrix.
	//
	inline TAbstractMatrix();

	// Initializes from specified elements.
	//
	inline TAbstractMatrix(const Elements& other);

	inline TAbstractMatrix(const TAbstractMatrix& other);
	virtual inline ~TAbstractMatrix();

	// Converting the type of elements to another.
	//
	template<typename U>
	explicit inline TAbstractMatrix(const TAbstractMatrix<Matrix, U, M, N>& other);

protected:
	// Returns an identity matrix. This method is defined only for square matrices.
	//
	template<typename = std::enable_if_t<(M == N)>>
	static Matrix baseMakeIdentity();

	inline Matrix& initIdentity();

	inline Matrix mul(T rhs) const;
	inline Matrix mul(const DerivedMatrix& rhs) const;
	inline Matrix& mulLocal(T rhs);
	inline void mul(const Matrix& rhs, DerivedMatrix* const out_result) const;

	// TODO: inverse, determinant

	inline std::string toString() const;
};

}// end namespace ph

#include "Math/General/TGeneralMatrix.ipp"