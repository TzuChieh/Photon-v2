#pragma once

#include "Math/TMatrix3.h"
#include "Common/assertion.h"

namespace ph
{

template<typename T>
inline TMatrix3<T> TMatrix3<T>::makeIdentity()
{
	return TMatrix3().initIdentity();
}

template<typename T>
inline TMatrix3<T>::TMatrix3(const T value)
{
	m[0][0] = value; m[0][1] = value; m[0][2] = value;
	m[1][0] = value; m[1][1] = value; m[1][2] = value;
	m[2][0] = value; m[2][1] = value; m[2][2] = value;
}

template<typename T>
inline TMatrix3<T>::TMatrix3(const Elements& elements) : 
	m(elements)
{}

template<typename T>
template<typename U>
inline TMatrix3<T>::TMatrix3(const TMatrix3<U>& other)
{
	for(std::size_t i = 0; i < 3; ++i)
	{
		for(std::size_t j = 0; j < 3; ++j)
		{
			m[i][j] = static_cast<T>(other.m[i][j]);
		}
	}
}

template<typename T>
inline TMatrix3<T>& TMatrix3<T>::initIdentity()
{
	m[0][0] = 1; m[0][1] = 0; m[0][2] = 0;
	m[1][0] = 0; m[1][1] = 1; m[1][2] = 0;
	m[2][0] = 0; m[2][1] = 0; m[2][2] = 1;

	return *this;
}

template<typename T>
inline TMatrix3<T> TMatrix3<T>::mul(const TMatrix3& rhs) const
{
	TMatrix3 result;
	for(std::size_t i = 0; i < 3; ++i)
	{
		for(std::size_t j = 0; j < 3; ++j)
		{
			result.m[i][j] = m[i][0] * rhs.m[0][j] +
			                 m[i][1] * rhs.m[1][j] +
			                 m[i][2] * rhs.m[2][j];
		}
	}

	return result;
}

template<typename T>
inline TMatrix3<T> TMatrix3<T>::mul(const T value) const
{
	return TMatrix3(*this).mulLocal(value);
}

template<typename T>
inline void TMatrix3<T>::mul(const TMatrix3& rhs, TMatrix3* const out_result) const
{
	PH_ASSERT(out_result && out_result != this);

	for(std::size_t i = 0; i < 3; ++i)
	{
		for(std::size_t j = 0; j < 3; ++j)
		{
			out_result->m[i][j] = m[i][0] * rhs.m[0][j] +
			                      m[i][1] * rhs.m[1][j] +
			                      m[i][2] * rhs.m[2][j];
		}
	}
}

template<typename T>
inline TMatrix3<T>& TMatrix3<T>::mulLocal(const T value)
{
	m[0][0] *= value; m[0][1] *= value; m[0][2] *= value;
	m[1][0] *= value; m[1][1] *= value; m[1][2] *= value;
	m[2][0] *= value; m[2][1] *= value; m[2][2] *= value;

	return *this;
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

	result.mulLocal(1 / determinant());

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
inline std::string TMatrix3<T>::toString() const
{
	std::string result;

	result += '[' + std::to_string(m[0][0]) + ", "
	              + std::to_string(m[0][1]) + ", "
	              + std::to_string(m[0][2]) + "]\n";

	result += '[' + std::to_string(m[1][0]) + ", "
	              + std::to_string(m[1][1]) + ", "
	              + std::to_string(m[1][2]) + "]\n";

	result += '[' + std::to_string(m[2][0]) + ", "
	              + std::to_string(m[2][1]) + ", "
	              + std::to_string(m[2][2]) + "]\n";

	result += '[' + std::to_string(m[3][0]) + ", "
	              + std::to_string(m[3][1]) + ", "
	              + std::to_string(m[3][2]) + "]";

	return result;
}

}// end namespace ph