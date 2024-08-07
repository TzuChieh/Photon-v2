#pragma once

#include "Math/TOrthonormalBasis3.h"
#include "Math/math.h"

#include <Common/assertion.h>

#include <algorithm>
#include <cmath>
#include <limits>

namespace ph::math
{

template<typename T>
inline TOrthonormalBasis3<T> TOrthonormalBasis3<T>::makeFromUnitY(const TVector3<T>& unitYAxis)
{
	PH_ASSERT_IN_RANGE(unitYAxis.lengthSquared(), static_cast<T>(0.9), static_cast<T>(1.1));

	// choose an axis deviate enough to specified y-axis to perform cross product in order to avoid some 
	// numerical errors
	TVector3<T> unitXAxis;
	if(std::abs(unitYAxis.y()) < constant::rcp_sqrt_2<real>)
	{
		unitXAxis.set({-unitYAxis.z(), T(0), unitYAxis.x()});// yAxis cross (0, 1, 0)
		unitXAxis.mulLocal(T(1) / std::sqrt(unitXAxis.x() * unitXAxis.x() + unitXAxis.z() * unitXAxis.z()));
	}
	else
	{
		unitXAxis.set({unitYAxis.y(), -unitYAxis.x(), T(0)});// yAxis cross (0, 0, 1)
		unitXAxis.mulLocal(T(1) / std::sqrt(unitXAxis.x() * unitXAxis.x() + unitXAxis.y() * unitXAxis.y()));
	}

	const TVector3<T> unitZAxis = unitXAxis.cross(unitYAxis);

	PH_ASSERT_IN_RANGE(unitXAxis.lengthSquared(), static_cast<T>(0.9), static_cast<T>(1.1));
	PH_ASSERT_IN_RANGE(unitZAxis.lengthSquared(), static_cast<T>(0.9), static_cast<T>(1.1));

	return TOrthonormalBasis3(unitXAxis, unitYAxis, unitZAxis);

	// TEST
	/*std::cerr << std::setprecision(20);
	if(std::abs(out_unitXaxis->length() - 1.0_r) > 0.000001_r)
		std::cerr << out_unitXaxis->length() << std::endl;
	if(std::abs(out_unitZaxis->length() - 1.0_r) > 0.000001_r)
		std::cerr << out_unitZaxis->length() << std::endl;
	if(out_unitXaxis->dot(*out_unitZaxis) > 0.000001_r)
		std::cerr << out_unitXaxis->dot(*out_unitZaxis) << std::endl;
	if(out_unitZaxis->dot(unitYaxis) > 0.000001_r)
		std::cerr << out_unitZaxis->dot(unitYaxis) << std::endl;
	if(unitYaxis.dot(*out_unitXaxis) > 0.000001_r)
		std::cerr << unitYaxis.dot(*out_unitXaxis) << std::endl;*/
}

template<typename T>
inline TOrthonormalBasis3<T>::TOrthonormalBasis3() :
	m_xAxis(1, 0, 0),
	m_yAxis(0, 1, 0),
	m_zAxis(0, 0, 1)
{}

template<typename T>
inline TOrthonormalBasis3<T>::TOrthonormalBasis3(
	const TVector3<T>& xAxis,
	const TVector3<T>& yAxis, 
	const TVector3<T>& zAxis)

	: m_xAxis(xAxis)
	, m_yAxis(yAxis)
	, m_zAxis(zAxis)
{
	PH_ASSERT_IN_RANGE(xAxis.lengthSquared(), static_cast<T>(0.9), static_cast<T>(1.1));
	PH_ASSERT_IN_RANGE(yAxis.lengthSquared(), static_cast<T>(0.9), static_cast<T>(1.1));
	PH_ASSERT_IN_RANGE(zAxis.lengthSquared(), static_cast<T>(0.9), static_cast<T>(1.1));
}

template<typename T>
inline TVector3<T> TOrthonormalBasis3<T>::worldToLocal(const TVector3<T>& worldVec) const
{
	return TVector3<T>(m_xAxis.dot(worldVec),
	                   m_yAxis.dot(worldVec), 
	                   m_zAxis.dot(worldVec));
}

template<typename T>
inline TVector3<T> TOrthonormalBasis3<T>::localToWorld(const TVector3<T>& localVec) const
{
	return m_xAxis.mul(localVec.x()).add(
	       m_yAxis.mul(localVec.y())).add(
	       m_zAxis.mul(localVec.z()));
}

template<typename T>
inline T TOrthonormalBasis3<T>::cosPhi(const TVector3<T>& unitVec) const
{
	const T zProjection = unitVec.dot(m_zAxis);
	const T rProjection = sinTheta(unitVec);
	return math::safe_clamp(zProjection / rProjection, static_cast<T>(-1), static_cast<T>(1));
}

template<typename T>
inline T TOrthonormalBasis3<T>::sinPhi(const TVector3<T>& unitVec) const
{
	const T xProjection = unitVec.dot(m_xAxis);
	const T rProjection = sinTheta(unitVec);
	return math::safe_clamp(xProjection / rProjection, static_cast<T>(-1), static_cast<T>(1));
}

template<typename T>
inline T TOrthonormalBasis3<T>::tanPhi(const TVector3<T>& unitVec) const
{
	const T zProjection = unitVec.dot(m_zAxis);
	const T xProjection = unitVec.dot(m_xAxis);
	const T tanP = xProjection / zProjection;
	return std::isfinite(tanP) ? tanP : static_cast<T>(0);
}

template<typename T>
inline T TOrthonormalBasis3<T>::cos2Phi(const TVector3<T>& unitVec) const
{
	return math::squared(cosPhi(unitVec));
}

template<typename T>
inline T TOrthonormalBasis3<T>::sin2Phi(const TVector3<T>& unitVec) const
{
	return math::squared(sinPhi(unitVec));
}

template<typename T>
inline T TOrthonormalBasis3<T>::tan2Phi(const TVector3<T>& unitVec) const
{
	const T cos2P = cos2Phi(unitVec);
	const T sin2P = 1 - cos2P;
	const T tan2P = sin2P / cos2P;
	PH_ASSERT(tan2P >= static_cast<T>(0) || !std::isfinite(tan2P));
	return std::isfinite(tan2P) ? tan2P : static_cast<T>(0);
}

template<typename T>
inline T TOrthonormalBasis3<T>::cosTheta(const TVector3<T>& unitVec) const
{
	return math::clamp(m_yAxis.dot(unitVec), static_cast<T>(-1), static_cast<T>(1));
}

template<typename T>
inline T TOrthonormalBasis3<T>::sinTheta(const TVector3<T>& unitVec) const
{
	return std::sqrt(sin2Theta(unitVec));
}

template<typename T>
inline T TOrthonormalBasis3<T>::tanTheta(const TVector3<T>& unitVec) const
{
	const T cosT = cosTheta(unitVec);
	const T tanT = sinTheta(unitVec) / cosT;
	return std::isfinite(tanT) ? tanT : static_cast<T>(0);
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
	return math::squared(cosTheta(unitVec));
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
	const T tan2T = sin2T / cos2T;
	PH_ASSERT(tan2T >= static_cast<T>(0) || !std::isfinite(tan2T));
	return std::isfinite(tan2T) ? tan2T : static_cast<T>(0);
}

template<typename T>
inline TOrthonormalBasis3<T>& TOrthonormalBasis3<T>::renormalize()
{
	renormalizeXAxis();
	renormalizeYAxis();
	renormalizeZAxis();

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

}// end namespace ph::math
