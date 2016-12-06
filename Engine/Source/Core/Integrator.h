#pragma once

namespace ph
{

class Intersector;
class Ray;
class Vector3f;

class Integrator
{
public:
	virtual ~Integrator() = 0;

	virtual void update(const Intersector& intersector) = 0;
	virtual void radianceAlongRay(const Ray& ray, const Intersector& intersector, Vector3f* const out_radiance) const = 0;
};

}// end namespace ph