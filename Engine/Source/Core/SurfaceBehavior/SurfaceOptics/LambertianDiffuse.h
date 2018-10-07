#pragma once

#include "Core/SurfaceBehavior/SurfaceOptics.h"
#include "Core/Texture/TTexture.h"
#include "Core/Quantity/SpectralStrength.h"

#include <memory>

namespace ph
{

class LambertianDiffuse : public SurfaceOptics
{
public:
	LambertianDiffuse();

	void setAlbedo(const std::shared_ptr<TTexture<SpectralStrength>>& albedo);

private:
	void calcBsdf(
		const SurfaceHit&         X,
		const Vector3R&           L,
		const Vector3R&           V,
		const SidednessAgreement& sidedness,
		SpectralStrength*         out_bsdf) const override;

	void calcBsdfSample(
		const SurfaceHit&         X,
		const Vector3R&           V,
		const SidednessAgreement& sidedness,
		Vector3R*                 out_L,
		SpectralStrength*         out_pdfAppliedBsdf) const override;

	void calcBsdfSamplePdfW(
		const SurfaceHit&         X,
		const Vector3R&           L,
		const Vector3R&           V,
		const SidednessAgreement& sidedness,
		real*                     out_pdfW) const override;

private:
	std::shared_ptr<TTexture<SpectralStrength>> m_albedo;
};

}// end namespace ph