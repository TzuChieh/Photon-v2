#pragma once

#include "Math/math_fwd.h"

#include <string>
#include <array>

namespace ph
{

template<typename T>
class TMatrix2 final
{
public:
	std::array<std::array<T, 2>, 2> m;

	inline TMatrix2();
	explicit inline TMatrix2(T value);
	inline TMatrix2(T m00, T m01, T m10, T m11);
	inline TMatrix2(const TMatrix2& other);

	template<typename U>
	explicit inline TMatrix2(const TMatrix2<U>& other);

	inline TMatrix2& initIdentity();
	inline TMatrix2 mul(const TMatrix2& rhs) const;
	inline TMatrix2 mul(T value) const;
	inline void mul(const TMatrix2& rhs, TMatrix2* out_result) const;
	inline TMatrix2& mulLocal(T value);
	inline TMatrix2 inverse() const;
	inline T determinant() const;

	inline bool solve(const TVector2<T>& b, TVector2<T>* out_x) const;
	inline bool solve(const TVector3<T>& bx, const TVector3<T>& by, 
	                  TVector3<T>* out_xx, TVector3<T>* out_xy) const;

	inline std::string toString() const;
};

}// end namespace ph

#include "Math/TMatrix2.ipp"