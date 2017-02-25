#pragma once

#include "Core/SurfaceBehavior/BSDF.h"
#include "Actor/Texture/Texture.h"
#include "Actor/Texture/ConstantTexture.h"

#include <memory>

namespace ph
{

class LambertianDiffuse final : public BSDF
{
public:
	LambertianDiffuse();
	virtual ~LambertianDiffuse() override;

	virtual void genImportanceSample(SurfaceSample& sample) const override;
	virtual real calcImportanceSamplePdfW(const SurfaceSample& sample) const override;
	virtual void evaluate(SurfaceSample& sample) const override;

	void setAlbedo(const std::shared_ptr<Texture>& albedo);

private:
	virtual void evaluate(const Intersection& X, const Vector3R& L, const Vector3R& V,
	                      Vector3R* const out_bsdf, ESurfacePhenomenon* const out_type) const override;
	virtual void genSample(const Intersection& X, const Vector3R& V,
	                       Vector3R* const out_L, Vector3R* const out_pdfAppliedBsdf, ESurfacePhenomenon* const out_type) const override;
	virtual void calcSampleDirPdfW(const Intersection& X, const Vector3R& L, const Vector3R& V, const ESurfacePhenomenon& type,
	                               real* const out_pdfW) const override;

private:
	std::shared_ptr<Texture> m_albedo;
};

}// end namespace ph