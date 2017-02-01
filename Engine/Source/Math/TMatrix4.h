#pragma once

#include "Common/primitive_type.h"
#include "Math/math_fwd.h"

#include <string>

namespace ph
{
	
template<typename T>
class TMatrix4
{
public:
	T m[4][4];

public:
	TMatrix4();

	inline TMatrix4& initIdentity();
	inline TMatrix4& initTranslation(const T x, const T y, const T z);
	inline TMatrix4& initTranslation(const TVector3<T>& value);
	inline TMatrix4& initRotation(const TQuaternion<T>& rot);
	inline TMatrix4& initScale(const T x, const T y, const T z);
	inline TMatrix4& initScale(const TVector3<T>& scale);
	inline TMatrix4& initPerspectiveProjection(const T fov,
	                                           const T aspectRatio,
	                                           const T zNear,
	                                           const T zFar);

	inline TMatrix4 mul(const TMatrix4& rhs) const;
	inline void mul(const TMatrix4& rhs, TMatrix4* const out_result) const;
	inline void mul(const TVector3<T>& rhsXYZ, const T rhsW, TVector3<T>* const out_result) const;
	inline TMatrix4& mulLocal(const T rhs);

	inline TMatrix4& inverse(TMatrix4* const out_result) const;
	inline T determinant() const;

	inline std::string toStringFormal() const;
};

}// end namespace ph