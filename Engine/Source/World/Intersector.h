#pragma once

namespace ph
{

class Intersector
{
public:
	virtual ~Intersector() = 0;

	//bool isIntersecting(const Ray& ray, Intersection* out_intersection) const;
};

}// end namespace ph