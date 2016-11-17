#pragma once

namespace ph
{

class World;
class Ray;
class Vector3f;

class Integrator
{
public:
	virtual ~Integrator() = 0;

	virtual void cook(const World& world) = 0;
	virtual void radianceAlongRay(const Ray& ray, const World& world, Vector3f* const out_radiance) const = 0;
};

}// end namespace ph