#pragma once

#include "Core/SurfaceBehavior/BSDF.h"
#include "Actor/Texture/Texture.h"

#include <memory>

namespace ph
{

class TranslucentMicrofacet final : public BSDF
{
public:
	TranslucentMicrofacet();
	virtual ~TranslucentMicrofacet() override;

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
	virtual void evaluate(const Intersection& X, const Vector3R& L, const Vector3R& V,
	                      SpectralStrength* out_bsdf, ESurfacePhenomenon* out_type) const override;
	virtual void genSample(const Intersection& X, const Vector3R& V,
	                       Vector3R* out_L, SpectralStrength* out_pdfAppliedBsdf, ESurfacePhenomenon* out_type) const override;
	virtual void calcSampleDirPdfW(const Intersection& X, const Vector3R& L, const Vector3R& V, const ESurfacePhenomenon& type,
	                               real* out_pdfW) const override;

private:
	std::shared_ptr<Texture> m_F0;
	std::shared_ptr<Texture> m_IOR;
	std::shared_ptr<Texture> m_alpha;
};

}// end namespace ph