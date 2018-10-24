#pragma once

#include "Core/SurfaceBehavior/SurfaceOptics.h"
#include "Math/TVector3.h"
#include "Core/Texture/TTexture.h"
#include "Core/SurfaceBehavior/Property/Microfacet.h"
#include "Core/SurfaceBehavior/Property/FresnelEffect.h"
#include "Core/Quantity/SpectralStrength.h"

#include <memory>

namespace ph
{

class OpaqueMicrofacet : public SurfaceOptics
{
public:
	OpaqueMicrofacet(
		const std::shared_ptr<FresnelEffect>& fresnel,
		const std::shared_ptr<Microfacet>&    microfacet);

	/*inline void setAlbedo(const std::shared_ptr<TTexture<SpectralStrength>>& albedo)
	{
		m_albedo = albedo;
	}*/

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
	//std::shared_ptr<TTexture<SpectralStrength>> m_albedo;
	std::shared_ptr<Microfacet>                 m_microfacet;
	std::shared_ptr<FresnelEffect>              m_fresnel;
};

}// end namespace ph