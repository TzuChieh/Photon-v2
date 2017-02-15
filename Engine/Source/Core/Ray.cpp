#include "Ray.h"

#include <limits>

namespace ph
{

const real Ray::MAX_T = std::numeric_limits<real>::max();

Ray::Ray(const Vector3R& origin, const Vector3R& direction, const real minT, const real maxT) :
	m_origin(origin), m_direction(direction), m_minT(minT), m_maxT(maxT)
{

}

Ray::Ray(const Vector3R& origin, const Vector3R& direction) :
	Ray(origin, direction, 0.0001_r, MAX_T)
{

}

Ray::Ray() : 
	Ray(Vector3R(0, 0, 0), Vector3R(0, 0, -1))
{

}

}// end namespace ph