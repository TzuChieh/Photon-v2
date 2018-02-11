#pragma once

#include "Core/SurfaceBehavior/SurfaceOptics.h"
#include "Core/Texture/TTexture.h"
#include "Core/Quantity/SpectralStrength.h"

#include <memory>

namespace ph
{

class LambertianDiffuse final : public SurfaceOptics
{
public:
	LambertianDiffuse();
	virtual ~LambertianDiffuse() override;

	void setAlbedo(const std::shared_ptr<TTexture<SpectralStrength>>& albedo);

private:
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

private:
	std::shared_ptr<TTexture<SpectralStrength>> m_albedo;
};

}// end namespace ph