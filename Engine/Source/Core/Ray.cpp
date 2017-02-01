#include "Ray.h"

namespace ph
{
Ray::Ray(const Vector3R& origin, const Vector3R& direction, const real minT, const real maxT) :
	m_origin(origin), m_direction(direction), m_minT(minT), m_maxT(maxT)
{

}

Ray::Ray(const Vector3R& origin, const Vector3R& direction) :
	Ray(origin, direction, RAY_T_EPSILON, RAY_T_MAX)
{

}

Ray::Ray() : 
	Ray(Vector3R(0, 0, 0), Vector3R(0, 0, -1))
{

}

}// end namespace ph