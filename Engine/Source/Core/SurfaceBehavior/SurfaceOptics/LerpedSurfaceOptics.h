#pragma once

#include "Core/SurfaceBehavior/SurfaceOptics.h"
#include "Core/Texture/texture_fwd.h"
#include "Core/Quantity/SpectralStrength.h"
#include "Common/primitive_type.h"
#include "Core/Texture/TSampler.h"

#include <memory>

namespace ph
{

class LerpedSurfaceOptics final : public SurfaceOptics
{
public:
	LerpedSurfaceOptics(
		const std::shared_ptr<SurfaceOptics>& optics0, 
		const std::shared_ptr<SurfaceOptics>& optics1);
	LerpedSurfaceOptics(
		const std::shared_ptr<SurfaceOptics>& optics0,
		const std::shared_ptr<SurfaceOptics>& optics1,
		real ratio);
	LerpedSurfaceOptics(
		const std::shared_ptr<SurfaceOptics>& optics0, 
		const std::shared_ptr<SurfaceOptics>& optics1,
		const std::shared_ptr<TTexture<SpectralStrength>>& ratio);
	virtual ~LerpedSurfaceOptics() override;

private:
	std::shared_ptr<SurfaceOptics>              m_optics0;
	std::shared_ptr<SurfaceOptics>              m_optics1;
	std::shared_ptr<TTexture<SpectralStrength>> m_ratio;
	TSampler<SpectralStrength>                  m_sampler;

	virtual void evalBsdf(
		const SurfaceHit& X, const Vector3R& L, const Vector3R& V,
		SpectralStrength* out_bsdf,
		ESurfacePhenomenon* out_type) const override;

	virtual void genBsdfSample(
		const SurfaceHit& X, const Vector3R& V,
		Vector3R* out_L,
		SpectralStrength* out_pdfAppliedBsdf,
		ESurfacePhenomenon* out_type) const override;

	virtual void calcBsdfSamplePdf(
		const SurfaceHit& X, const Vector3R& L, const Vector3R& V,
		const ESurfacePhenomenon& type,
		real* out_pdfW) const override;

	static real pickOptics0Probability(const SpectralStrength& ratio);
};

}// end namespace ph