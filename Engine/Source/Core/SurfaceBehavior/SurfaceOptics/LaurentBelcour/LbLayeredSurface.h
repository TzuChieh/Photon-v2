#pragma once

#include "Core/SurfaceBehavior/SurfaceOptics.h"

namespace ph
{

class LbLayeredSurface : public SurfaceOptics
{
public:
	LbLayeredSurface();
	~LbLayeredSurface() override;

private:
	void evalBsdf(
		const SurfaceHit& X, const Vector3R& L, const Vector3R& V,
		SpectralStrength* out_bsdf) const override;

	void genBsdfSample(
		const SurfaceHit& X, const Vector3R& V,
		Vector3R* out_L,
		SpectralStrength* out_pdfAppliedBsdf) const override;

	void calcBsdfSamplePdf(
		const SurfaceHit& X, const Vector3R& L, const Vector3R& V,
		real* out_pdfW) const override;
};

}// end namespace ph