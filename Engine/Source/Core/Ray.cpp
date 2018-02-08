#include "Ray.h"

#include <limits>

namespace ph
{

Ray::Ray(const Vector3R& origin, const Vector3R& direction, 
         const real minT, const real maxT, 
         const Time& time) :
	m_origin(origin), m_direction(direction), 
	m_minT(minT), m_maxT(maxT),
	m_time(time),
	m_differential()
{

}

Ray::Ray(const Vector3R& origin, const Vector3R& direction,
         const real minT, const real maxT) :
	Ray(origin, direction, minT, maxT, Time())
{

}

Ray::Ray(const Vector3R& origin, const Vector3R& direction) :
	Ray(origin, direction, 0.0_r, std::numeric_limits<real>::max())
{

}

Ray::Ray() : 
	Ray(Vector3R(0, 0, 0), Vector3R(0, 0, -1))
{

}

Ray::Differential::Differential() : 
	Differential(Vector3R(0), Vector3R(0), Vector3R(0), Vector3R(0))
{

}

Ray::Differential::Differential(const Vector3R& dOdX, const Vector3R& dOdY,
                                const Vector3R& dDdX, const Vector3R& dDdY) : 
	m_dOdX(dOdX), m_dOdY(dOdY),
	m_dDdX(dDdX), m_dDdY(dDdY)
{

}

}// end namespace ph