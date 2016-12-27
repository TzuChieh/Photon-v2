#pragma once

#include "Core/SurfaceBehavior/BSDFcos.h"
#include "Math/Vector3f.h"

namespace ph
{

class SLight : public BSDFcos
{
public:
	SLight();
	virtual ~SLight() override;

	virtual void genImportanceSample(const Intersection& intersection, const Ray& ray, SurfaceSample* const out_sample) const override;

	inline void setEmittedRadiance(const float32 r, const float32 g, const float32 b)
	{
		m_emittedRadiance.set(r, g, b);
	}

private:
	Vector3f m_emittedRadiance;
};

}// end namespace ph