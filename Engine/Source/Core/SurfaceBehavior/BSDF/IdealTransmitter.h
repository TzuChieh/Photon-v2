#pragma once

#include "Core/SurfaceBehavior/BSDF.h"
#include "Core/SurfaceBehavior/Utility/DielectricFresnel.h"

#include <memory>

namespace ph
{

class IdealTransmitter : public BSDF
{
public:
	IdealTransmitter();
	virtual ~IdealTransmitter() override;

	inline void setFresnelEffect(const std::shared_ptr<DielectricFresnel>& fresnel)
	{
		m_fresnel = fresnel;
	}

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

private:
	std::shared_ptr<DielectricFresnel> m_fresnel;
};

}// end namespace ph