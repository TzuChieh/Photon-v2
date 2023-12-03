#pragma once

#include "Math/math_fwd.h"

#include <cstddef>
#include <array>
#include <string>

namespace ph::math
{

template<typename T>
class TMatrix2 final
{
public:
	static TMatrix2 makeIdentity();

public:
	using Elements = std::array<std::array<T, 2>, 2>;

	Elements m;

	inline TMatrix2() = default;
	explicit TMatrix2(T value);
	TMatrix2(T m00, T m01, T m10, T m11);
	TMatrix2(const TVector2<T>& m00m01, const TVector2<T>& m10m11);
	inline TMatrix2(const TMatrix2& other) = default;

	template<typename U>
	explicit TMatrix2(const TMatrix2<U>& other);

	TMatrix2& initIdentity();
	TMatrix2 mul(const TMatrix2& rhs) const;
	TMatrix2 mul(T value) const;
	void mul(const TMatrix2& rhs, TMatrix2* out_result) const;
	TMatrix2& mulLocal(T value);
	TMatrix2 inverse() const;
	T determinant() const;

	// TODO: make solve() methods accepts all "subscriptable" types to (potentially) 
	// improve performance & less code duplications

	/*! @brief Solves linear systems of the form Ax = b.

	Treating this matrix as A.

	@param b A 2x1 vector.
	@param[out] out_x A 2x1 vector.
	@return If x is successfully solved, method returns `true` and @p out_x 
	stores the answer; otherwise, `false` is returned and what @p out_x stores
	is undefined.
	*/
	bool solve(
		const std::array<T, 2>& b,
		std::array<T, 2>*       out_x) const;

	/*! @brief Solves linear systems of the form Ax = b.

	See solve(const std::array<T, 2>&, std::array<T, 2>*) for details.
	*/
	bool solve(
		const TVector2<T>& b,
		TVector2<T>*       out_x) const;

	/*! @brief Solve N linear systems of the form Ax = b.

	Treating this matrix as A, linear systems A*x_i = b_i (0 <= i < N) are 
	solved. This is equivalent to having @p bs as a `2` by `N` matrix and 
	passing them in column-major (@p out_xs follows similarly, except it's 
	for outputs).

	@param A A 2x2 matrix.
	@param bs `N` 2x1 vectors (or a 2xN matrix in column-major).
	@param[out] out_xs `N` 2x1 vectors (or a 2xN matrix in column-major).
	@return If x is successfully solved, method returns `true` and @p out_xs 
	stores the answer; otherwise, `false` is returned and what @p out_xs
	stores is undefined.
	*/
	template<std::size_t N>
	bool solve(
		const std::array<std::array<T, 2>, N>& bs, 
		std::array<std::array<T, 2>, N>*       out_xs) const;

	std::string toString() const;
};

}// end namespace ph::math

#include "Math/TMatrix2.ipp"
