#pragma once

#include <vector>
#include <memory>

namespace ph
{

class Primitive;
class Intersection;
class Ray;

class World final
{
public:
	void addPrimitive(const std::shared_ptr<Primitive>& primitive);
	bool isIntersecting(const Ray& ray, Intersection* out_intersection) const;

private:
	std::vector<std::shared_ptr<Primitive>> m_primitives;
};

}// end namespace ph