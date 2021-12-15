#pragma once

#include "Core/Intersectable/Intersector.h"

#include <vector>
#include <memory>

namespace ph
{

class Intersectable;

class BruteForceIntersector final : public Intersector
{
public:
	virtual ~BruteForceIntersector() override;

	virtual void update(const CookedDataStorage& cookedActors) override;
	virtual bool isIntersecting(const Ray& ray, HitProbe& probe) const override;
	virtual bool isOccluding(const Ray& ray) const override;
	virtual math::AABB3D calcAABB() const override;

private:
	std::vector<const Intersectable*> m_intersectables;
};

}// end namespace ph
