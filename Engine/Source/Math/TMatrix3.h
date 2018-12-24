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
	static TMatrix3 makeIdentity();

public:
	using Elements = std::array<std::array<T, 3>, 3>;

	Elements m;

	inline TMatrix3() = default;
	explicit TMatrix3(T value);
	explicit TMatrix3(const Elements& elements);
	inline TMatrix3(const TMatrix3& other) = default;

	template<typename U>
	explicit TMatrix3(const TMatrix3<U>& other);

	TMatrix3& initIdentity();

	TMatrix3 mul(T value) const;
	TMatrix3& mulLocal(T value);
	TMatrix3 mul(const TMatrix3& rhs) const;
	void mul(const TMatrix3& rhs, TMatrix3* out_result) const;

	TMatrix3 inverse() const;
	T determinant() const;

	std::string toString() const;
};

}// end namespace ph

#include "Math/TMatrix3.ipp"