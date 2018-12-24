#pragma once

#include "Math/TMatrix2.h"
#include "Math/TVector2.h"
#include "Math/TVector3.h"
#include "Common/assertion.h"

#include <limits>
#include <string>
#include <cmath>

namespace ph
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
inline TMatrix2<T>::TMatrix2(const T m00, const T m01, const T m10, const T m11)
{
	m[0][0] = m00; m[0][1] = m01;
	m[1][0] = m10; m[1][1] = m11;
}

template<typename T>
template<typename U>
inline TMatrix2<T>::TMatrix2(const TMatrix2<U>& other) :
	TMatrix2(static_cast<T>(other.m[0][0]), static_cast<T>(other.m[0][1]), 
	         static_cast<T>(other.m[1][0]), static_cast<T>(other.m[1][1]))
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
inline bool TMatrix2<T>::solve(const TVector2<T>& b, TVector2<T>* const out_x) const
{
	PH_ASSERT(!std::numeric_limits<T>::is_integer);

	const T det     = determinant();
	const T reciDet = det != static_cast<T>(0) ? static_cast<T>(1) / det
	                                           : std::numeric_limits<T>::infinity();
	if(std::fabs(reciDet) >= std::numeric_limits<T>::max())
	{
		return false;
	}
	PH_ASSERT(reciDet != std::numeric_limits<T>::infinity());
	PH_ASSERT(reciDet == reciDet);

	out_x->x = (m[1][1] * b.x - m[0][1] * b.y) * reciDet;
	out_x->y = (m[0][0] * b.y - m[1][0] * b.x) * reciDet;
	return true;
}

template<typename T>
inline bool TMatrix2<T>::solve(
	const TVector3<T>& bx, const TVector3<T>& by,
	TVector3<T>* const out_xx, TVector3<T>* const out_xy) const
{
	PH_ASSERT(!std::numeric_limits<T>::is_integer);

	const T det     = determinant();
	const T reciDet = det != static_cast<T>(0) ? static_cast<T>(1) / det
	                                           : std::numeric_limits<T>::infinity();
	if(std::fabs(reciDet) >= std::numeric_limits<T>::max())
	{
		return false;
	}
	PH_ASSERT_MSG(std::isfinite(reciDet), std::to_string(reciDet));

	out_xx->x = (m[1][1] * bx.x - m[0][1] * by.x) * reciDet;
	out_xy->x = (m[0][0] * by.x - m[1][0] * bx.x) * reciDet;
	out_xx->y = (m[1][1] * bx.y - m[0][1] * by.y) * reciDet;
	out_xy->y = (m[0][0] * by.y - m[1][0] * bx.y) * reciDet;
	out_xx->z = (m[1][1] * bx.z - m[0][1] * by.z) * reciDet;
	out_xy->z = (m[0][0] * by.z - m[1][0] * bx.z) * reciDet;
	return true;
}

template<typename T>
inline std::string TMatrix2<T>::toString() const
{
	return "[" + std::to_string(m[0][0]) + ", " + std::to_string(m[0][1]) + "]" + 
	       "[" + std::to_string(m[1][0]) + ", " + std::to_string(m[1][1]) + "]";
}

}// end namespace ph