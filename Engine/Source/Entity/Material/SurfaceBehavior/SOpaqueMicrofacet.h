#pragma once

#include "Entity/Material/SurfaceBehavior/SurfaceBehavior.h"
#include "Math/Vector3f.h"
#include "Image/Texture.h"
#include "Image/ConstantTexture.h"

#include <memory>

namespace ph
{

class SOpaqueMicrofacet : public SurfaceBehavior
{
public:
	SOpaqueMicrofacet();
	virtual ~SOpaqueMicrofacet() override;

	virtual void genBsdfCosImportanceSample(const Intersection& intersection, const Ray& ray, SurfaceSample* const out_sample) const override;

	inline void setAlbedo(const std::shared_ptr<Texture>& albedo)
	{
		m_albedo = albedo;
	}

	inline void setRoughness(const std::shared_ptr<Texture>& roughness)
	{
		m_roughness = roughness;
	}

	inline void setF0(const std::shared_ptr<Texture>& f0)
	{
		m_F0 = f0;
	}

private:
	std::shared_ptr<Texture> m_albedo;
	std::shared_ptr<Texture> m_roughness;
	std::shared_ptr<Texture> m_F0;
};

}// end namespace ph