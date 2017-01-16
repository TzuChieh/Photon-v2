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
	virtual float32 calcImportanceSamplePdfW(const SurfaceSample& sample) const override;
	virtual void evaluate(SurfaceSample& sample) const override;

	inline void setAlbedo(const std::shared_ptr<Texture>& albedo)
	{
		m_albedo = albedo;
	}

	inline void setAlpha(const std::shared_ptr<Texture>& alpha)
	{
		m_alpha = alpha;
	}

	inline void setF0(const std::shared_ptr<Texture>& f0)
	{
		m_F0 = f0;
	}

private:
	std::shared_ptr<Texture> m_albedo;
	std::shared_ptr<Texture> m_alpha;
	std::shared_ptr<Texture> m_F0;
};

}// end namespace ph