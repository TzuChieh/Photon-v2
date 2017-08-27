#pragma once

#include "Core/SurfaceBehavior/BSDF.h"

namespace ph
{

class IdealTransmitter : public BSDF
{
public:
	virtual ~IdealTransmitter() override;

private:
	virtual void evaluate(
		const Intersection& X, const Vector3R& L, const Vector3R& V,
		SpectralStrength* out_bsdf,
		ESurfacePhenomenon* out_type) const override;

	virtual void genSample(
		const Intersection& X, const Vector3R& V,
		Vector3R* out_L,
		SpectralStrength* out_pdfAppliedBsdf,
		ESurfacePhenomenon* out_type) const override;

	virtual void calcSampleDirPdfW(
		const Intersection& X, const Vector3R& L, const Vector3R& V,
		const ESurfacePhenomenon& type,
		real* out_pdfW) const override;
};

}// end namespace ph