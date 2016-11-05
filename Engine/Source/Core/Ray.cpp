#include "Ray.h"

namespace ph
{

Ray::Ray(const Vector3f& origin, const Vector3f& direction) : 
	m_origin(origin), m_direction(direction)
{

}

Ray::Ray() : 
	Ray(Vector3f(0, 0, 0), Vector3f(0, 0, -1))
{

}

}// end namespace ph