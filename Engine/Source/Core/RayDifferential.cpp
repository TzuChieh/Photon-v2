#include "Core/RayDifferential.h"
#include "Common/assertion.h"

#include <cmath>

namespace ph
{

RayDifferential::RayDifferential() :
	RayDifferential(math::Vector3R(0), math::Vector3R(0), math::Vector3R(0), math::Vector3R(0))
{}

RayDifferential::RayDifferential(const math::Vector3R& dPdX, const math::Vector3R& dPdY,
                                 const math::Vector3R& dDdX, const math::Vector3R& dDdY) :
	m_dPdX(dPdX), m_dPdY(dPdY),
	m_dDdX(dDdX), m_dDdY(dDdY),
	m_isPartialPsNonZero(false), m_isPartialDsNonZero(false)
{}

void RayDifferential::transferToSurface(const math::Vector3R& surfaceP, const math::Vector3R& surfaceN)
{
	PH_ASSERT(std::abs(surfaceN.length() - 1.0_r) < 0.0001_r);

	const real DoN = m_D.dot(surfaceN);
	const real T   = (surfaceP - m_P).dot(surfaceN) / DoN;
	PH_ASSERT(T >= 0.0_r);

	const real dTdX = -(m_dPdX + T * m_dDdX).dot(surfaceN) / DoN;
	const real dTdY = -(m_dPdY + T * m_dDdY).dot(surfaceN) / DoN;

	m_dPdX = (m_dPdX + T * m_dDdX) + dTdX * m_D;
	m_dPdY = (m_dPdY + T * m_dDdY) + dTdY * m_D;
}

}// end namespace ph
