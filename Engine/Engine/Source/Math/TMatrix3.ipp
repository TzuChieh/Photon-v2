#pragma once

#include "Math/TMatrix3.h"
#include "Common/assertion.h"
#include "Math/TVector3.h"

namespace ph::math
{

template<typename T>
template<typename U>
inline TMatrix3<T>::TMatrix3(const TMatrix3<U>& other)
{
	for(std::size_t ri = 0; ri < 3; ++ri)
	{
		for(std::size_t ci = 0; ci < 3; ++ci)
		{
			m[ri][ci] = static_cast<T>(other[ri][ci]);
		}
	}
}

template<typename T>
inline TMatrix3<T>::TMatrix3(
	const T m00, const T m01, const T m02,
	const T m10, const T m11, const T m12,
	const T m20, const T m21, const T m22)
{
	m[0][0] = m00; m[0][1] = m01; m[0][2] = m02;
	m[1][0] = m10; m[1][1] = m11; m[1][2] = m12;
	m[2][0] = m20; m[2][1] = m21; m[2][2] = m22;
}

template<typename T>
inline TMatrix3<T> TMatrix3<T>::inverse() const
{
	TMatrix3 result;

	result.m[0][0] =  (m[2][2] * m[1][1] - m[2][1] * m[1][2]);
	result.m[0][1] = -(m[2][2] * m[0][1] - m[2][1] * m[0][2]);
	result.m[0][2] =  (m[1][2] * m[0][1] - m[1][1] * m[0][2]);

	result.m[1][0] = -(m[2][2] * m[1][0] - m[2][0] * m[1][2]);
	result.m[1][1] =  (m[2][2] * m[0][0] - m[2][0] * m[0][2]);
	result.m[1][2] = -(m[1][2] * m[0][0] - m[1][0] * m[0][2]);

	result.m[2][0] =  (m[2][1] * m[1][0] - m[2][0] * m[1][1]);
	result.m[2][1] = -(m[2][1] * m[0][0] - m[2][0] * m[0][1]);
	result.m[2][2] =  (m[1][1] * m[0][0] - m[1][0] * m[0][1]);

	const T det = determinant();
	PH_ASSERT_NE(det, 0);
	result.mulLocal(static_cast<T>(1) / det);

	return result;
}

template<typename T>
inline T TMatrix3<T>::determinant() const
{
	return m[0][0] * ((m[1][1] * m[2][2]) - (m[2][1] * m[1][2])) -
	       m[0][1] * (m[1][0] * m[2][2] - m[2][0] * m[1][2]) +
	       m[0][2] * (m[1][0] * m[2][1] - m[2][0] * m[1][1]);
}

template<typename T>
inline TVector3<T> TMatrix3<T>::mul(const TVector3<T>& rhsColVector) const
{
	return TVector3<T>(this->multiplyVector(rhsColVector.toArray()));
}

template<typename T>
inline TVector3<T> TMatrix3<T>::operator * (const TVector3<T>& rhsColVector) const
{
	return this->mul(rhsColVector);
}

}// end namespace ph::math
