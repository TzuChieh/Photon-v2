#pragma once

#include "Core/SurfaceBehavior/BSDFcos.h"
#include "Math/Vector3f.h"
#include "Image/Texture.h"
#include "Image/ConstantTexture.h"

#include <memory>

namespace ph
{

class OpaqueMicrofacet : public BSDFcos
{
public:
	OpaqueMicrofacet();
	virtual ~OpaqueMicrofacet() override;

	virtual void genImportanceSample(SurfaceSample& sample) const override;
	virtual void evaluate(const Intersection& intersection, const Vector3f& L, const Vector3f& V, Vector3f* const out_value) const override;

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