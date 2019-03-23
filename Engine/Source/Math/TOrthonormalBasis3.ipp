#pragma once

#include "Math/TOrthonormalBasis3.h"
#include "Math/math.h"

#include <algorithm>
#include <cmath>

namespace ph
{

template<typename T>
inline TOrthonormalBasis3<T>::TOrthonormalBasis3() :
	m_xAxis(1, 0, 0),
	m_yAxis(0, 1, 0),
	m_zAxis(0, 0, 1)
{}

template<typename T>
inline TOrthonormalBasis3<T>::TOrthonormalBasis3(const TOrthonormalBasis3& other) :
	TOrthonormalBasis3(other.m_xAxis, other.m_yAxis, other.m_zAxis)
{}

template<typename T>
inline TOrthonormalBasis3<T>::TOrthonormalBasis3(
	const TVector3<T>& xAxis,
	const TVector3<T>& yAxis, 
	const TVector3<T>& zAxis) : 
	m_xAxis(xAxis),
	m_yAxis(yAxis),
	m_zAxis(zAxis)
{}

template<typename T>
inline TVector3<T> TOrthonormalBasis3<T>::worldToLocal(const TVector3<T>& worldVec) const
{
	return TVector3<T>(m_xAxis.dot(worldVec),
	                   m_yAxis.dot(worldVec), 
	                   m_zAxis.dot(worldVec));
}

template<typename T>
inline T TOrthonormalBasis3<T>::cosPhi(const TVector3<T>& unitVec) const
{
	const T cosT           = cosTheta(unitVec);
	TVector3<T> xzPlaneVec = unitVec.sub(m_yAxis.mul(cosT));
	const T length2        = xzPlaneVec.lengthSquared();

	if(length2 != 0)
	{
		xzPlaneVec.mulLocal(1 / std::sqrt(length2));
		return math::clamp<T>(xzPlaneVec.dot(m_zAxis), -1, 1);
	}
	else
	{
		return 1;
	}
}

template<typename T>
inline T TOrthonormalBasis3<T>::sinPhi(const TVector3<T>& unitVec) const
{
	return std::sqrt(sin2Phi(unitVec));
}

template<typename T>
inline T TOrthonormalBasis3<T>::tanPhi(const TVector3<T>& unitVec) const
{
	return std::sqrt(tan2Phi(unitVec));
}

template<typename T>
inline T TOrthonormalBasis3<T>::cos2Phi(const TVector3<T>& unitVec) const
{
	const T cosP = cosPhi(unitVec);
	return cosP * cosP;
}

template<typename T>
inline T TOrthonormalBasis3<T>::sin2Phi(const TVector3<T>& unitVec) const
{
	return 1 - cos2Phi(unitVec);
}

template<typename T>
inline T TOrthonormalBasis3<T>::tan2Phi(const TVector3<T>& unitVec) const
{
	const T cos2P = cos2Phi(unitVec);
	const T sin2P = 1 - cos2P;
	return sin2P / cos2P;
}

template<typename T>
inline T TOrthonormalBasis3<T>::cosTheta(const TVector3<T>& unitVec) const
{
	return math::clamp<T>(m_yAxis.dot(unitVec), -1, 1);
}

template<typename T>
inline T TOrthonormalBasis3<T>::sinTheta(const TVector3<T>& unitVec) const
{
	return std::sqrt(sin2Theta(unitVec));
}

template<typename T>
inline T TOrthonormalBasis3<T>::tanTheta(const TVector3<T>& unitVec) const
{
	return std::sqrt(tan2Theta(unitVec));
}

template<typename T>
inline T TOrthonormalBasis3<T>::absCosTheta(const TVector3<T>& unitVec) const
{
	return std::abs(cosTheta(unitVec));
}

template<typename T>
inline T TOrthonormalBasis3<T>::absSinTheta(const TVector3<T>& unitVec) const
{
	return std::abs(sinTheta(unitVec));
}

template<typename T>
inline T TOrthonormalBasis3<T>::cos2Theta(const TVector3<T>& unitVec) const
{
	const T cosT = cosTheta(unitVec);
	return cosT * cosT;
}

template<typename T>
inline T TOrthonormalBasis3<T>::sin2Theta(const TVector3<T>& unitVec) const
{
	return 1 - cos2Theta(unitVec);
}

template<typename T>
inline T TOrthonormalBasis3<T>::tan2Theta(const TVector3<T>& unitVec) const
{
	const T cos2T = cos2Theta(unitVec);
	const T sin2T = 1 - cos2T;
	return sin2T / cos2T;
}

template<typename T>
inline TOrthonormalBasis3<T>& TOrthonormalBasis3<T>::renormalize()
{
	m_xAxis.normalizeLocal();
	m_yAxis.normalizeLocal();
	m_zAxis.normalizeLocal();

	return *this;
}

template<typename T>
inline TOrthonormalBasis3<T>& TOrthonormalBasis3<T>::renormalizeXAxis()
{
	m_xAxis.normalizeLocal();

	return *this;
}

template<typename T>
inline TOrthonormalBasis3<T>& TOrthonormalBasis3<T>::renormalizeYAxis()
{
	m_yAxis.normalizeLocal();

	return *this;
}

template<typename T>
inline TOrthonormalBasis3<T>& TOrthonormalBasis3<T>::renormalizeZAxis()
{
	m_zAxis.normalizeLocal();

	return *this;
}

template<typename T>
inline TOrthonormalBasis3<T>& TOrthonormalBasis3<T>::setXAxis(const TVector3<T>& axis)
{
	m_xAxis = axis;

	return *this;
}

template<typename T>
inline TOrthonormalBasis3<T>& TOrthonormalBasis3<T>::setYAxis(const TVector3<T>& axis)
{
	m_yAxis = axis;

	return *this;
}

template<typename T>
inline TOrthonormalBasis3<T>& TOrthonormalBasis3<T>::setZAxis(const TVector3<T>& axis)
{
	m_zAxis = axis;

	return *this;
}

template<typename T>
inline TOrthonormalBasis3<T>& TOrthonormalBasis3<T>::set(
	const TVector3<T>& xAxis,
	const TVector3<T>& yAxis,
	const TVector3<T>& zAxis)
{
	m_xAxis = xAxis;
	m_yAxis = yAxis;
	m_zAxis = zAxis;

	return *this;
}

template<typename T>
inline TVector3<T> TOrthonormalBasis3<T>::getXAxis() const
{
	return m_xAxis;
}

template<typename T>
inline TVector3<T> TOrthonormalBasis3<T>::getYAxis() const
{
	return m_yAxis;
}

template<typename T>
inline TVector3<T> TOrthonormalBasis3<T>::getZAxis() const
{
	return m_zAxis;
}

template<typename T>
inline TOrthonormalBasis3<T>& TOrthonormalBasis3<T>::operator = (const TOrthonormalBasis3& rhs)
{
	set(rhs.m_xAxis, rhs.m_yAxis, rhs.m_zAxis);

	return *this;
}

}// end namespace ph