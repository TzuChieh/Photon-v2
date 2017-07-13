#pragma once

#include "Core/Intersectable/Primitive.h"

#include <memory>

namespace ph
{

//class EmitablePrimitive : public Primitive
//{
//public:
//	EmitablePrimitive(std::unique_ptr<Primitive> primitive);
//	virtual ~EmitablePrimitive() override;
//
//	virtual bool isIntersecting(const Ray& ray, 
//	                            Intersection* out_intersection) const override;
//	virtual bool isIntersecting(const Ray& ray) const override;
//	virtual bool isIntersectingVolumeConservative(const AABB& aabb) const override;
//	virtual void calcAABB(AABB* out_aabb) const override;
//	virtual real calcPositionSamplePdfA(const Vector3R& position) const override;
//	virtual void genPositionSample(PositionSample* out_sample) const override;
//	virtual real calcExtendedArea() const override;
//};

}// end namespace ph