#pragma once

#include "Core/SurfaceBehavior/SurfaceOptics.h"
#include "Core/SurfaceBehavior/Utility/FresnelEffect.h"

#include <memory>

namespace ph
{

class IdealReflector : public SurfaceOptics
{
public:
	IdealReflector();
	virtual ~IdealReflector() override;

	inline void setFresnelEffect(const std::shared_ptr<FresnelEffect>& fresnel)
	{
		m_fresnel = fresnel;
	}

private:
	virtual void evalBsdf(
		const Intersection& X, const Vector3R& L, const Vector3R& V,
		SpectralStrength* out_bsdf,
		ESurfacePhenomenon* out_type) const override;

	virtual void genBsdfSample(
		const Intersection& X, const Vector3R& V,
		Vector3R* out_L,
		SpectralStrength* out_pdfAppliedBsdf,
		ESurfacePhenomenon* out_type) const override;

	virtual void calcBsdfSamplePdf(
		const Intersection& X, const Vector3R& L, const Vector3R& V,
		const ESurfacePhenomenon& type,
		real* out_pdfW) const override;

private:
	std::shared_ptr<FresnelEffect> m_fresnel;
};

}// end namespace ph