#pragma once

#include "Core/Intersectable/Intersectable.h"
#include "Math/Transform/Transform.h"

#include <memory>

namespace ph
{

class TransformedIntersectable : public Intersectable
{
public:
	TransformedIntersectable(std::unique_ptr<Intersectable> intersectable, 
	                         const Transform*               localToWorld, 
	                         const Transform*               worldToLocal);
	TransformedIntersectable(TransformedIntersectable&& other);
	virtual ~TransformedIntersectable() override;

	virtual bool isIntersecting(const Ray& ray, 
	                            Intersection* out_intersection) const override;
	virtual bool isIntersecting(const Ray& ray) const override;
	virtual bool isIntersectingVolumeConservative(const AABB3D& aabb) const override;
	virtual void calcAABB(AABB3D* out_aabb) const override;

	TransformedIntersectable& operator = (TransformedIntersectable&& rhs);

	// forbid copying
	TransformedIntersectable(const TransformedIntersectable& other) = delete;
	TransformedIntersectable& operator = (const TransformedIntersectable& rhs) = delete;

protected:
	std::unique_ptr<Intersectable> m_intersectable;
	const Transform*               m_localToWorld;
	const Transform*               m_worldToLocal;
};

}// end namespace ph