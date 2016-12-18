#include "Core/Intersection.h"

namespace ph
{

Intersection::Intersection() : 
	m_hitPosition(0, 0, 0), m_hitSmoothNormal(0, 0, -1), m_hitGeoNormal(0, 0, -1), m_hitUVW(0, 0, 0), m_hitPrimitive(nullptr)
{

}

}// end namespace ph