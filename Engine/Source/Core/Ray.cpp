#include "Ray.h"

namespace ph
{
Ray::Ray(const Vector3f& origin, const Vector3f& direction, const float32 minT, const float32 maxT) :
	m_origin(origin), m_direction(direction), m_minT(minT), m_maxT(maxT)
{

}

Ray::Ray(const Vector3f& origin, const Vector3f& direction) :
	Ray(origin, direction, RAY_T_EPSILON, RAY_T_MAX)
{

}

Ray::Ray() : 
	Ray(Vector3f(0, 0, 0), Vector3f(0, 0, -1))
{

}

}// end namespace ph