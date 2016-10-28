#include "Ray.h"

namespace ph
{

Ray::Ray(const Vector3f& origin, const Vector3f& direction) : 
	m_origin(origin), m_direction(direction), m_LiWeight(1, 1, 1), m_LiRadiance(0, 0, 0)
{

}

Ray::Ray() : 
	Ray(Vector3f(0, 0, 0), Vector3f(0, 0, -1))
{

}

void Ray::accumulateLiWeight(const Vector3f& newLiWeight)
{
	m_LiWeight.mulLocal(newLiWeight);
}

void Ray::addLiRadiance(const Vector3f& newLiRadiance)
{
	m_LiRadiance.addLocal(newLiRadiance);
}

}// end namespace ph