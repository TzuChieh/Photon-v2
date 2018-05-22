#pragma once

#include "Math/math_fwd.h"

#include <string>
#include <array>

namespace ph
{

template<typename T>
class TMatrix3 final
{
public:
	typedef std::array<std::array<T, 3>, 3> Elements;

	Elements m;

	inline TMatrix3();
	explicit inline TMatrix3(T value);
	explicit inline TMatrix3(const Elements& elements);
	inline TMatrix3(const TMatrix3& other);

	template<typename U>
	explicit inline TMatrix3(const TMatrix3<U>& other);

	inline TMatrix3& initIdentity();

	inline TMatrix3 mul(T value) const;
	inline TMatrix3& mulLocal(T value);
	inline TMatrix3 mul(const TMatrix3& rhs) const;
	inline void mul(const TMatrix3& rhs, TMatrix3* out_result) const;

	inline TMatrix3 inverse() const;
	inline T determinant() const;

	inline std::string toString() const;
};

}// end namespace ph

#include "Math/TMatrix3.ipp"