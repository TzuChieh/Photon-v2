#pragma once

#include "Entity/Material/SurfaceBehavior/SurfaceBehavior.h"

namespace ph
{

class SPerfectMirror : public SurfaceBehavior
{
public:
	virtual ~SPerfectMirror() override;

	virtual void genBsdfCosImportanceSample(const Intersection& intersection, const Ray& ray, SurfaceSample* const out_sample) const override;
};

}// end namespace ph