#pragma once

#include "Core/Intersectable/Intersectable.h"
#include "Math/Transform/Transform.h"

#include <memory>

namespace ph
{

class TransformedIntersectable : public Intersectable
{
public:
	TransformedIntersectable(const Intersectable* intersectable, 
	                         const Transform*     localToWorld, 
	                         const Transform*     worldToLocal);
	TransformedIntersectable(const TransformedIntersectable& other);
	virtual ~TransformedIntersectable() override;

	virtual bool isIntersecting(const Ray& ray, HitProbe& probe) const override;
	virtual void calcIntersectionDetail(const Ray& ray, HitProbe& probe,
	                                    HitDetail* out_detail) const override;
	virtual bool isIntersecting(const Ray& ray) const override;
	virtual bool isIntersectingVolumeConservative(const AABB3D& aabb) const override;
	virtual void calcAABB(AABB3D* out_aabb) const override;

	TransformedIntersectable& operator = (const TransformedIntersectable& rhs);

protected:
	const Intersectable* m_intersectable;
	const Transform*     m_localToWorld;
	const Transform*     m_worldToLocal;
};

}// end namespace ph