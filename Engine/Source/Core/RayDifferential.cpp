#include "Core/RayDifferential.h"

namespace ph
{

RayDifferential::RayDifferential() :
	RayDifferential(Vector3R(0), Vector3R(0), Vector3R(0), Vector3R(0))
{

}

RayDifferential::RayDifferential(const Vector3R& dPdX, const Vector3R& dPdY,
                                const Vector3R& dDdX, const Vector3R& dDdY) : 
	m_dPdX(dPdX), m_dPdY(dPdY),
	m_dDdX(dDdX), m_dDdY(dDdY),
	m_isPartialPsNonZero(false), m_isPartialDsNonZero(false)
{

}

}// end namespace ph