#pragma once

#include "Math/TMatrix2.h"
#include "Math/TVector2.h"
#include "Math/TVector3.h"

#include <Common/assertion.h>

#include <limits>
#include <string>
#include <cmath>

namespace ph::math
{

template<typename T>
inline TMatrix2<T> TMatrix2<T>::makeIdentity() 
{
	return TMatrix2().initIdentity();
}

template<typename T>
inline TMatrix2<T>::TMatrix2(const T value) : 
	TMatrix2(value, value, value, value)
{}

template<typename T>
inline TMatrix2<T>::TMatrix2(const T m00, const T m01, const T m10, const T m11) : 
	m{m00, m01,
	  m10, m11}
{}

template<typename T>
inline TMatrix2<T>::TMatrix2(const TVector2<T>& m00m01, const TVector2<T>& m10m11) : 
	m{m00m01.x(), m00m01.y(),
	  m10m11.x(), m10m11.y()}
{}

template<typename T>
template<typename U>
inline TMatrix2<T>::TMatrix2(const TMatrix2<U>& other) :
	m{static_cast<T>(other.m[0][0]), static_cast<T>(other.m[0][1]),
	  static_cast<T>(other.m[1][0]), static_cast<T>(other.m[1][1])}
{}

template<typename T>
inline TMatrix2<T>& TMatrix2<T>::initIdentity()
{
	m[0][0] = static_cast<T>(1); m[0][1] = static_cast<T>(0);
	m[1][0] = static_cast<T>(0); m[1][1] = static_cast<T>(1);

	return *this;
}

template<typename T>
inline TMatrix2<T> TMatrix2<T>::mul(const TMatrix2& rhs) const
{
	TMatrix2 result;
	this->mul(rhs, &result);
	return result;
}

template<typename T>
inline TMatrix2<T> TMatrix2<T>::mul(const T value) const
{
	return TMatrix2(*this).mulLocal(value);
}

template<typename T>
inline void TMatrix2<T>::mul(const TMatrix2& rhs, TMatrix2* const out_result) const
{
	out_result->m[0][0] = m[0][0] * rhs.m[0][0] + m[0][1] * rhs.m[1][0];
	out_result->m[0][1] = m[0][0] * rhs.m[0][1] + m[0][1] * rhs.m[1][1];
	out_result->m[1][0] = m[1][0] * rhs.m[0][0] + m[1][1] * rhs.m[1][0];
	out_result->m[1][1] = m[1][0] * rhs.m[0][1] + m[1][1] * rhs.m[1][1];
}

template<typename T>
inline TMatrix2<T>& TMatrix2<T>::mulLocal(const T value)
{
	m[0][0] *= value;
	m[0][1] *= value;
	m[1][0] *= value;
	m[1][1] *= value;

	return *this;
}

template<typename T>
inline TMatrix2<T> TMatrix2<T>::inverse() const
{
	return TMatrix2( m[1][1], -m[0][1], 
	                -m[1][0],  m[0][0]).mulLocal(static_cast<T>(1) / determinant());
}

template<typename T>
inline T TMatrix2<T>::determinant() const
{
	return m[0][0] * m[1][1] - m[1][0] * m[0][1];
}

template<typename T>
inline bool TMatrix2<T>::solve(
	const std::array<T, 2>& b,
	std::array<T, 2>* const out_x) const
{
	PH_ASSERT(out_x);

	std::array<std::array<T, 2>, 1> x;
	if(solve<1>({b}, &x))
	{
		(*out_x)[0] = x[0][0];
		(*out_x)[1] = x[0][1];

		return true;
	}
	else
	{
		return false;
	}
}

template<typename T>
inline bool TMatrix2<T>::solve(
	const TVector2<T>& b,
	TVector2<T>* const out_x) const
{
	PH_ASSERT(out_x);

	std::array<T, 2> x;
	if(solve({b.x(), b.y()}, &x))
	{
		out_x->x() = x[0];
		out_x->y() = x[1];

		return true;
	}
	else
	{
		return false;
	}
}

template<typename T>
template<std::size_t N>
inline bool TMatrix2<T>::solve(
	const std::array<std::array<T, 2>, N>& bs,
	std::array<std::array<T, 2>, N>* const out_xs) const
{
	static_assert(!std::numeric_limits<T>::is_integer);

	const T det = determinant();
	if(det == 0)
	{
		return false;
	}

	const T rcpDet = static_cast<T>(1) / determinant();

	PH_ASSERT(out_xs);
	for(std::size_t i = 0; i < N; ++i)
	{
		(*out_xs)[i][0] = (m[1][1] * bs[i][0] - m[0][1] * bs[i][1]) * rcpDet;
		(*out_xs)[i][1] = (m[0][0] * bs[i][1] - m[1][0] * bs[i][0]) * rcpDet;
	}

	for(std::size_t i = 0; i < N; ++i)
	{
		if(!std::isfinite((*out_xs)[i][0]) || !std::isfinite((*out_xs)[i][1]))
		{
			return false;
		}
	}
	return true;
}

template<typename T>
inline std::string TMatrix2<T>::toString() const
{
	return "[" + std::to_string(m[0][0]) + ", " + std::to_string(m[0][1]) + "]" + 
	       "[" + std::to_string(m[1][0]) + ", " + std::to_string(m[1][1]) + "]";
}

}// end namespace ph::math
