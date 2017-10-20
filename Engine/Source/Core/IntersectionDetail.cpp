#include "Core/IntersectionDetail.h"

#include <limits>

namespace ph
{

IntersectionDetail::IntersectionDetail() :
	m_hitPrimitive(nullptr), 
	m_hitPosition(0, 0, 0), 
	m_hitSmoothNormal(0, 0, -1), 
	m_hitGeoNormal(0, 0, -1), 
	m_hitUVW(0, 0, 0), 
	m_hitRayT(std::numeric_limits<real>::infinity())
{

}

}// end namespace ph