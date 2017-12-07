#include "Core/HitInfo.h"

#include <limits>

namespace ph
{

HitInfo::HitInfo() :
	m_position(0, 0, 0), 
	m_rayT(std::numeric_limits<real>::infinity()),

	m_dPdU(1, 0, 0),
	m_dPdV(0, 0, 1),
	m_dNdU(1, 0, 0),
	m_dNdV(0, 0, 1),

	m_geometryBasis(),
	m_shadingBasis()
{

}

}// end namespace ph