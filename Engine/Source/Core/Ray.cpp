#include "Ray.h"

#include <limits>

namespace ph
{

Ray::Ray(const Vector3R& origin, const Vector3R& direction, 
         const real minT, const real maxT, 
         const Time& time) :
	m_origin(origin), m_direction(direction), 
	m_minT(minT), m_maxT(maxT),
	m_time(time)
{

}

Ray::Ray(const Vector3R& origin, const Vector3R& direction,
         const real minT, const real maxT) :
	Ray(origin, direction, minT, maxT, Time())
{

}

Ray::Ray(const Vector3R& origin, const Vector3R& direction) :
	Ray(origin, direction, 0, std::numeric_limits<real>::max())
{

}

Ray::Ray() : 
	Ray(Vector3R(0, 0, 0), Vector3R(0, 0, -1))
{

}

}// end namespace ph