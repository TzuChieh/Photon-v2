#pragma once

#include <vector>
#include <memory>

namespace ph
{

class Ray;
class Intersection;
class Triangle;

class Intersector
{
public:
	virtual ~Intersector() = 0;

	virtual void construct() = 0;
	virtual bool isIntersecting(const Ray& ray, Intersection* out_intersection) const = 0;

	void clearData();
	void addTriangle(const Triangle& triangle);

protected:
	std::vector<Triangle> m_triangles;
};

}// end namespace ph