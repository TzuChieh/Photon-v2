#pragma once

#include "Core/SurfaceBehavior/BSDF.h"
#include "Math/TVector3.h"
#include "Actor/Texture/Texture.h"
#include "Actor/Texture/ConstantTexture.h"

#include <memory>

namespace ph
{

class OpaqueMicrofacet final : public BSDF
{
public:
	OpaqueMicrofacet();
	virtual ~OpaqueMicrofacet() override;

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
	virtual void evaluate(const Intersection& X, const Vector3R& L, const Vector3R& V,
	                      SpectralStrength* out_bsdf, 
	                      ESurfacePhenomenon* out_type) const override;

	virtual void genSample(const Intersection& X, const Vector3R& V,
	                       Vector3R* out_L, 
	                       SpectralStrength* out_pdfAppliedBsdf, 
	                       ESurfacePhenomenon* out_type) const override;

	virtual void calcSampleDirPdfW(const Intersection& X, const Vector3R& L, const Vector3R& V, 
	                               const ESurfacePhenomenon& type,
	                               real* out_pdfW) const override;

private:
	std::shared_ptr<Texture> m_albedo;
	std::shared_ptr<Texture> m_alpha;
	std::shared_ptr<Texture> m_F0;
};

}// end namespace ph