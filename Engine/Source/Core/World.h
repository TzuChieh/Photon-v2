#pragma once

#include <vector>
#include <memory>

namespace ph
{

class Geometry;
class Intersection;
class Ray;

class World final
{
public:
	void addGeometry(const std::shared_ptr<Geometry>& geometry);
	bool isIntersecting(const Ray& ray, Intersection* out_intersection) const;

private:
	std::vector<std::shared_ptr<Geometry>> m_geometries;
};

}// end namespace ph