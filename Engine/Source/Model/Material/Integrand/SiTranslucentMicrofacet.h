#pragma once

#include "Model/Material/Integrand/SurfaceIntegrand.h"
#include "Image/Texture.h"

#include <memory>

namespace ph
{

class SiTranslucentMicrofacet : public SurfaceIntegrand
{
public:
	SiTranslucentMicrofacet();
	virtual ~SiTranslucentMicrofacet() override;

	virtual void evaluateImportanceSample(const Intersection& intersection, const Ray& ray, SurfaceSample* const out_sample) const override;

	inline void setF0(const std::shared_ptr<Texture>& f0)
	{
		m_F0 = f0;
	}

	inline void setIOR(const std::shared_ptr<Texture>& ior)
	{
		m_IOR = ior;
	}

	inline void setRoughness(const std::shared_ptr<Texture>& roughness)
	{
		m_roughness = roughness;
	}

private:
	std::shared_ptr<Texture> m_F0;
	std::shared_ptr<Texture> m_IOR;
	std::shared_ptr<Texture> m_roughness;
};

}// end namespace ph