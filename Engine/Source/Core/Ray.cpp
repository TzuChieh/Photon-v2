#include "Ray.h"

namespace ph
{

Ray::Ray(const Vector3f& origin, const Vector3f& direction) : 
	m_origin(origin), m_direction(direction)
{

}

}// end namespace ph