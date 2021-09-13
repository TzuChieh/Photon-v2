#pragma once

#include "Math/General/TMatrixNBase.h"
#include "Math/math_fwd.h"

#include <string>
#include <array>

namespace ph::math
{

template<typename T>
class TMatrix3 final : public TMatrixNBase<TMatrix3<T>, T, 3>
{
private:
	using Base = TMatrixNBase<TMatrix3<T>, T, 3>;

protected:
	using Base::m;

public:
	inline TMatrix3() = default;
	inline TMatrix3(const TMatrix3& other) = default;
	inline TMatrix3(TMatrix3&& other) = default;
	inline TMatrix3& operator = (const TMatrix3& rhs) = default;
	inline TMatrix3& operator = (TMatrix3&& rhs) = default;

	using Base::Base;

	template<typename U>
	explicit TMatrix3(const TMatrix3<U>& other);

public:
	using RowVec3 = Base::RowVecN;
	using ColVec3 = Base::ColVecM;

	template<std::size_t K>
	using TMatrix3xK = Base::template TMatrixNxK<K>;

	template<std::size_t K>
	using TMatrixKx3 = Base::template TMatrixKxN<K>;

	using Matrix3x3 = Base::MatrixNxM;

	void mul(const TVector3<T>& rhsColVector, TVector3<T>* out_result) const;

	ColVec3 multiplyVector(const RowVec3& rhsColVector) const;

	template<std::size_t K>
	void multiplyMatrix(const TMatrix3xK<K>& rhsMatrix, TMatrix3xK<K>* out_result) const;

	template<std::size_t K>
	void multiplyTransposedMatrix(const TMatrixKx3<K>& rhsMatrix, TMatrix3xK<K>* out_result) const;

	TMatrix3 inverse() const;
	T determinant() const;

	using Base::mul;
};

}// end namespace ph::math

#include "Math/TMatrix3.ipp"
