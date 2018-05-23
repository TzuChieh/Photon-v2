#pragma once

#include "Common/primitive_type.h"
#include "Math/math_fwd.h"

#include <string>
#include <array>

namespace ph
{
	
template<typename T>
class TMatrix4
{
public:
	static TMatrix4 IDENTITY();

public:
	typedef std::array<std::array<T, 4>, 4> Elements;

	Elements m;

public:
	inline TMatrix4();
	explicit inline TMatrix4(T value);
	explicit inline TMatrix4(const Elements& elements);
	inline TMatrix4(const TMatrix4& other);
	virtual inline ~TMatrix4();

	template<typename U>
	explicit inline TMatrix4(const TMatrix4<U>& other);

	inline TMatrix4& initIdentity();
	inline TMatrix4& initTranslation(T x, T y, T z);
	inline TMatrix4& initTranslation(const TVector3<T>& value);
	inline TMatrix4& initRotation(const TQuaternion<T>& rot);
	inline TMatrix4& initRotation(const TVector3<T>& orthBasisX, const TVector3<T>& orthBasisY, const TVector3<T>& orthBasisZ);
	inline TMatrix4& initScale(T x, T y, T z);
	inline TMatrix4& initScale(const TVector3<T>& scale);
	inline TMatrix4& initPerspectiveProjection(T fov,
	                                           T aspectRatio,
	                                           T zNear,
	                                           T zFar);

	inline TMatrix4 mul(const TMatrix4& rhs) const;
	inline void mul(const TMatrix4& rhs, TMatrix4* out_result) const;
	inline void mul(const TVector3<T>& rhsXYZ, T rhsW, TVector3<T>* out_result) const;
	inline TMatrix4& mulLocal(T rhs);

	inline TMatrix4& inverse(TMatrix4* out_result) const;
	inline TMatrix4 transpose() const;

	inline T determinant() const;

	inline std::string toString() const;
};

}// end namespace ph

#include "Math/TMatrix4.ipp"