#include "Core/IntersectionDetail.h"

#include <limits>

namespace ph
{

IntersectionDetail::IntersectionDetail() :
	m_primitive(nullptr), 
	m_position(0, 0, 0), 
	m_uvw(0, 0, 0), 
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