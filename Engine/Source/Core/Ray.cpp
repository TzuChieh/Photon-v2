#include "Ray.h"

namespace ph
{

Ray::Ray(const Vector3f& origin, const Vector3f& direction) : 
	m_origin(origin), m_direction(direction)
{

}

Ray::Ray() : 
	m_origin(0, 0, 0), m_direction(0, 0, -1)
{

}

}// end namespace ph