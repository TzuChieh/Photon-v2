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

	virtual void update(const std::vector<std::unique_ptr<Primitive>>& primitives) override;
	virtual bool isIntersecting(const Ray& ray, Intersection* out_intersection) const override;

private:
	std::vector<const Primitive*> m_primitives;
};

}// end namespace ph