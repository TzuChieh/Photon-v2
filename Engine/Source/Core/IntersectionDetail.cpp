#include "Core/IntersectionDetail.h"

namespace ph
{

IntersectionDetail::IntersectionDetail() :
	m_hitPrimitive(nullptr), 
	m_hitPosition(0, 0, 0), 
	m_hitSmoothNormal(0, 0, -1), 
	m_hitGeoNormal(0, 0, -1), 
	m_hitUVW(0, 0, 0), 
	m_hitRayT(0.0_r)
{

}

}// end namespace ph