#pragma once

#include "Core/SurfaceBehavior/SurfaceOptics.h"
#include "Core/Texture/Texture.h"
#include "Core/Texture/ConstantTexture.h"

#include <memory>

namespace ph
{

class LambertianDiffuse final : public SurfaceOptics
{
public:
	LambertianDiffuse();
	virtual ~LambertianDiffuse() override;

	void setAlbedo(const std::shared_ptr<Texture>& albedo);

private:
	virtual void evalBsdf(
		const HitDetail& X, const Vector3R& L, const Vector3R& V,
		SpectralStrength* out_bsdf,
		ESurfacePhenomenon* out_type) const override;

	virtual void genBsdfSample(
		const HitDetail& X, const Vector3R& V,
		Vector3R* out_L,
		SpectralStrength* out_pdfAppliedBsdf,
		ESurfacePhenomenon* out_type) const override;

	virtual void calcBsdfSamplePdf(
		const HitDetail& X, const Vector3R& L, const Vector3R& V,
		const ESurfacePhenomenon& type,
		real* out_pdfW) const override;

private:
	std::shared_ptr<Texture> m_albedo;
};

}// end namespace ph