#pragma once

#include "Math/TOrthonormalBasis3.h"
#include "Math/math.h"
#include "Common/assertion.h"

#include <algorithm>
#include <cmath>
#include <limits>

namespace ph::math
{

template<typename T>
inline TOrthonormalBasis3<T> TOrthonormalBasis3<T>::makeFromUnitY(const TVector3<T>& unitYAxis)
{
	PH_ASSERT_MSG(unitYAxis.length() > T(0.9) && unitYAxis.isFinite(), unitYAxis.toString());

	// choose an axis deviate enough to specified y-axis to perform cross product in order to avoid some 
	// numeric errors
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

	PH_ASSERT_MSG(unitXAxis.isFinite() && unitZAxis.isFinite(), "\n"
		"unit-x-axis = " + unitXAxis.toString() + "\n"
		"unit-z-axis = " + unitZAxis.toString() + "\n");

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

	if(rProjection != T(0))
	{
		return math::clamp(zProjection / rProjection, T(-1), T(1));
	}
	else
	{
		// corresponds to phi = 0
		return T(1);
	}
}

template<typename T>
inline T TOrthonormalBasis3<T>::sinPhi(const TVector3<T>& unitVec) const
{
	const T xProjection = unitVec.dot(m_xAxis);
	const T rProjection = sinTheta(unitVec);

	if(rProjection != T(0))
	{
		return math::clamp(xProjection / rProjection, T(-1), T(1));
	}
	else
	{
		// corresponds to phi = 0
		return T(0);
	}
}

template<typename T>
inline T TOrthonormalBasis3<T>::tanPhi(const TVector3<T>& unitVec) const
{
	const T zProjection = unitVec.dot(m_zAxis);
	const T xProjection = unitVec.dot(m_xAxis);

	if(zProjection != T(0))
	{
		return math::clamp(
			xProjection / zProjection,
			std::numeric_limits<T>::lowest(),
			std::numeric_limits<T>::max());
	}
	else
	{
		switch(math::sign(xProjection))
		{
		case -1: return std::numeric_limits<T>::lowest();
		case  0: return T(0);
		case  1: return std::numeric_limits<T>::max();
		}

		PH_ASSERT_UNREACHABLE_SECTION();
		return T(0);
	}
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

	if(cos2P != T(0))
	{
		return math::clamp(
			sin2P / cos2P,
			std::numeric_limits<T>::lowest(),
			std::numeric_limits<T>::max());
	}
	else
	{
		return std::numeric_limits<T>::max();
	}
}

template<typename T>
inline T TOrthonormalBasis3<T>::cosTheta(const TVector3<T>& unitVec) const
{
	return math::clamp(m_yAxis.dot(unitVec), T(-1), T(1));
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

	if(cosT != T(0))
	{
		return math::clamp(
			sinTheta(unitVec) / cosT,
			std::numeric_limits<T>::lowest(),
			std::numeric_limits<T>::max());
	}
	else
	{
		switch(math::sign(sinTheta(unitVec)))
		{
		case -1: return std::numeric_limits<T>::lowest();
		case  0: return T(0);
		case  1: return std::numeric_limits<T>::max();
		}

		PH_ASSERT_UNREACHABLE_SECTION();
		return T(0);
	}
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

	if(cos2T != T(0))
	{
		return math::clamp(
			sin2T / cos2T,
			std::numeric_limits<T>::lowest(),
			std::numeric_limits<T>::max());
	}
	else
	{
		return std::numeric_limits<T>::max();
	}
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

}// end namespace ph::math
