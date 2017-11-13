#include "Core/IntersectionDetail.h"

#include <limits>

namespace ph
{

IntersectionDetail::IntersectionDetail() :
	m_primitive(nullptr), 
	m_position(0, 0, 0), 
	m_smoothNormal(0, 0, -1), 
	m_geoNormal(0, 0, -1), 
	m_uvw(0, 0, 0), 
	m_rayT(std::numeric_limits<real>::infinity()),

	m_dPdU(1, 0, 0),
	m_dPdV(0, 0, 1)
{

}

}// end namespace ph