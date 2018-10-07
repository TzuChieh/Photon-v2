#pragma once

#include "Core/SurfaceBehavior/SurfaceOptics.h"

#include <memory>

namespace ph
{

/*
	An ideal absorber absorbs any incoming energy. Pretty much resembles
	a black hole under event horizon.
*/
class IdealAbsorber : public SurfaceOptics
{
public:
	IdealAbsorber();

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
};

}// end namespace ph