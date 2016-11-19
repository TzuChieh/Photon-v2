#include "Core/Intersection.h"

namespace ph
{

Intersection::Intersection() : 
	m_hitPosition(0, 0, 0), m_hitNormal(0, 0, -1), m_hitPrimitive(nullptr)
{

}

}// end namespace ph