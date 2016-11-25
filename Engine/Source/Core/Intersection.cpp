#include "Core/Intersection.h"

namespace ph
{

Intersection::Intersection() : 
	m_hitPosition(0, 0, 0), m_hitNormal(0, 0, -1), m_hitUVW(0, 0, 0), m_hitTriangle(nullptr)
{

}

}// end namespace ph