#pragma once

#include "Model/Material/Integrand/SurfaceIntegrand.h"
#include "Math/Vector3f.h"
#include "Image/Texture.h"
#include "Image/ConstantTexture.h"

#include <memory>

namespace ph
{

class SiLambertianDiffuse : public SurfaceIntegrand
{
public:
	SiLambertianDiffuse();
	virtual ~SiLambertianDiffuse() override;

	virtual void genUniformRandomV(const Intersection& intersection, const Vector3f& L, Vector3f* out_V) const override;
	virtual void genImportanceRandomV(const Intersection& intersection, const Vector3f& L, Vector3f* out_V) const override;
	virtual void evaluateUniformRandomVPDF(const Intersection& intersection, const Vector3f& L, const Vector3f& V, Vector3f* const out_PDF) const override;
	virtual void evaluateImportanceRandomVPDF(const Intersection& intersection, const Vector3f& L, const Vector3f& V, Vector3f* const out_PDF) const override;
	virtual void evaluateLiWeight(const Intersection& intersection, const Vector3f& L, const Vector3f& V, Vector3f* const out_LiWeight) const override;
	
	virtual void evaluateImportanceSample(const Intersection& intersection, const Ray& ray, SurfaceSample* const out_sample) const override;

	void setAlbedo(const std::shared_ptr<Texture>& albedo);

private:
	std::shared_ptr<Texture> m_albedo;
};

}// end namespace ph