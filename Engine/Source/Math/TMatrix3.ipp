#pragma once

#include "Math/TMatrix3.h"
#include "Common/assertion.h"

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

}// end namespace ph::math
