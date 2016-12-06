#pragma once

#include "World/Intersector.h"

#include <vector>
#include <memory>

namespace ph
{

class Triangle;

class BruteForceIntersector final : public Intersector
{
public:
	virtual ~BruteForceIntersector() override;

	virtual void construct() override;
	virtual bool isIntersecting(const Ray& ray, Intersection* out_intersection) const override;
};

}// end namespace ph