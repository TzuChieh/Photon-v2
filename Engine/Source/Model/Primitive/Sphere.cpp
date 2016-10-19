#include "Model/Primitive/Sphere.h"
#include "Core/Ray.h"
#include "Core/Intersection.h"

#include <cmath>

#define SPHERE_EPSILON 0.0001f

namespace ph
{

Sphere::Sphere(const Vector3f& center, const float32 radius) :
	m_center(center), m_radius(radius)
{

}

bool Sphere::isIntersecting(const Ray& ray, Intersection* const out_intersection) const
{
	// ray origin:         o
	// ray direction:      d
	// sphere center:      c
	// sphere radius:      r
	// intersection point: p
	// vector dot:         *
	// ray equation:       o + td (t is a scalar variable)
	// To find the intersection point, the length of vector (op - oc) must equal to r.
	// Equation: |op - oc| = r, since op = td, we can write (td - oc) * (td - oc) = r^2.
	// After simplifying we got: t^2(d*d) - 2t(d*oc) + (oc*oc) - r^2 = 0.
	// (notice that d*d is in fact 1)

	Vector3f oc = m_center.sub(ray.getOrigin());     // vector from ray origin to sphere center
	float32 b = ray.getDirection().dot(oc);          // b in the quadratic equation above (-2 can be cancelled out while solving t)
	float32 D = b*b - oc.dot(oc) + m_radius*m_radius;// quadratic equation's discriminant

	if(D < 0.0f)
	{
		return false;
	}
	else
	{
		D = sqrt(D);

		// pick closest point in front of ray origin
		float32 t;
		t = ((t = b - D) > SPHERE_EPSILON ? t : ((t = b + D) > SPHERE_EPSILON ? t : 0.0f));

		if(t > 0.0f)
		{
			out_intersection->setHitPosition(ray.getDirection().mul(t).addLocal(ray.getOrigin()));
			out_intersection->setHitNormal(out_intersection->getHitPosition().sub(m_center).divLocal(m_radius));

			return true;
		}
		else
		{
			return false;
		}
	}
}

}// end namespace ph