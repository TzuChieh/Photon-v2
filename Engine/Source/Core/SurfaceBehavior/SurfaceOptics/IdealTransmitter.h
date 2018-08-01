#pragma once

#include "Core/SurfaceBehavior/SurfaceOptics.h"
#include "Core/SurfaceBehavior/Property/DielectricFresnel.h"

#include <memory>

namespace ph
{

class IdealTransmitter : public SurfaceOptics
{
public:
	IdealTransmitter();
	virtual ~IdealTransmitter() override;

	inline void setFresnelEffect(const std::shared_ptr<DielectricFresnel>& fresnel)
	{
		m_fresnel = fresnel;
	}

private:
	virtual void evalBsdf(
		const SurfaceHit&         X,
		const Vector3R&           L,
		const Vector3R&           V,
		const SidednessAgreement& sidedness,
		SpectralStrength*         out_bsdf) const override;

	virtual void genBsdfSample(
		const SurfaceHit&         X,
		const Vector3R&           V,
		const SidednessAgreement& sidedness,
		Vector3R*                 out_L,
		SpectralStrength*         out_pdfAppliedBsdf) const override;

	virtual void calcBsdfSamplePdf(
		const SurfaceHit&         X,
		const Vector3R&           L,
		const Vector3R&           V,
		const SidednessAgreement& sidedness,
		real*                     out_pdfW) const override;

private:
	std::shared_ptr<DielectricFresnel> m_fresnel;
};

}// end namespace ph