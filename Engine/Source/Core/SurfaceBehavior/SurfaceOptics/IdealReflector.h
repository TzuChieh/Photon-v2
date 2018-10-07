#pragma once

#include "Core/SurfaceBehavior/SurfaceOptics.h"
#include "Core/SurfaceBehavior/Property/FresnelEffect.h"

#include <memory>

namespace ph
{

class IdealReflector : public SurfaceOptics
{
public:
	IdealReflector();

	inline void setFresnelEffect(const std::shared_ptr<FresnelEffect>& fresnel)
	{
		m_fresnel = fresnel;
	}

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
	std::shared_ptr<FresnelEffect> m_fresnel;
};

}// end namespace ph