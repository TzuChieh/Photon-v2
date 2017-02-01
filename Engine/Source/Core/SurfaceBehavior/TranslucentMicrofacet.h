#pragma once

#include "Core/SurfaceBehavior/BSDFcos.h"
#include "Actor/Texture/Texture.h"

#include <memory>

namespace ph
{

class TranslucentMicrofacet : public BSDFcos
{
public:
	TranslucentMicrofacet();
	virtual ~TranslucentMicrofacet() override;

	virtual void genImportanceSample(SurfaceSample& sample) const override;
	virtual real calcImportanceSamplePdfW(const SurfaceSample& sample) const override;
	virtual void evaluate(SurfaceSample& sample) const override;

	inline void setF0(const std::shared_ptr<Texture>& f0)
	{
		m_F0 = f0;
	}

	inline void setIOR(const std::shared_ptr<Texture>& ior)
	{
		m_IOR = ior;
	}

	inline void setAlpha(const std::shared_ptr<Texture>& alpha)
	{
		m_alpha = alpha;
	}

private:
	std::shared_ptr<Texture> m_F0;
	std::shared_ptr<Texture> m_IOR;
	std::shared_ptr<Texture> m_alpha;
};

}// end namespace ph