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

	result.mulLocal(static_cast<T>(1) / determinant());

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
inline void TMatrix3<T>::mul(const TVector3<T>& rhsColVector, TVector3<T>* const out_result) const
{
	PH_ASSERT(out_result);
	PH_ASSERT(&rhsColVector != out_result);

	// TODO: use base impl

	out_result->x = m[0][0] * rhsColVector.x + m[0][1] * rhsColVector.y + m[0][2] * rhsColVector.z;
	out_result->y = m[1][0] * rhsColVector.x + m[1][1] * rhsColVector.y + m[1][2] * rhsColVector.z;
	out_result->z = m[2][0] * rhsColVector.x + m[2][1] * rhsColVector.y + m[2][2] * rhsColVector.z;
}

template<typename T>
inline auto TMatrix3<T>::multiplyVector(const RowVec3& rhsColVector) const
-> ColVec3
{
	return Base::multiplyVector(rhsColVector);
}

template<typename T>
template<std::size_t K>
inline void TMatrix3<T>::multiplyMatrix(const TMatrix3xK<K>& rhsMatrix, TMatrix3xK<K>* const out_result) const
{
	return Base::template multiplyMatrix<K>(rhsMatrix, out_result);
 }

template<typename T>
template<std::size_t K>
inline void TMatrix3<T>::multiplyTransposedMatrix(const TMatrixKx3<K>& rhsMatrix, TMatrix3xK<K>* const out_result) const
{
	return Base::template multiplyTransposedMatrix<K>(rhsMatrix, out_result);
}

}// end namespace ph::math
