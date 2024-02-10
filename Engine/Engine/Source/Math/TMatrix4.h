#pragma once

#include "Math/math_fwd.h"

#include <Common/primitive_type.h>

#include <string>
#include <array>

namespace ph::math
{
	
template<typename T>
class TMatrix4 final
{
public:
	static TMatrix4 makeIdentity();

public:
	using Elements = std::array<std::array<T, 4>, 4>;

	Elements m;

public:
	inline TMatrix4() = default;
	explicit TMatrix4(T value);
	explicit TMatrix4(const Elements& elements);
	inline TMatrix4(const TMatrix4& other) = default;

	template<typename U>
	explicit TMatrix4(const TMatrix4<U>& other);

	TMatrix4& initIdentity();
	TMatrix4& initTranslation(T x, T y, T z);
	TMatrix4& initTranslation(const TVector3<T>& value);
	TMatrix4& initRotation(const TQuaternion<T>& rot);
	TMatrix4& initRotation(const TVector3<T>& orthBasisX, const TVector3<T>& orthBasisY, const TVector3<T>& orthBasisZ);
	TMatrix4& initScale(T x, T y, T z);
	TMatrix4& initScale(const TVector3<T>& scale);
	TMatrix4& initPerspectiveProjection(T fov,
	                                    T aspectRatio,
	                                    T zNear,
	                                    T zFar);

	TMatrix4 mul(const TMatrix4& rhs) const;
	void mul(const TMatrix4& rhs, TMatrix4* out_result) const;
	void mul(const TVector3<T>& rhsXYZ, T rhsW, TVector3<T>* out_result) const;
	TMatrix4& mulLocal(T rhs);

	TMatrix4& inverse(TMatrix4* out_result) const;
	TMatrix4 transpose() const;

	T determinant() const;

	bool isEqual(const TMatrix4& other) const;
	std::string toString() const;
};

}// end namespace ph::math

#include "Math/TMatrix4.ipp"
