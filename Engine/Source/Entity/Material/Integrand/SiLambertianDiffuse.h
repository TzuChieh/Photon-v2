#pragma once

#include "Entity/Material/Integrand/SurfaceIntegrand.h"
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

	virtual void evaluateImportanceSample(const Intersection& intersection, const Ray& ray, SurfaceSample* const out_sample) const override;

	void setAlbedo(const std::shared_ptr<Texture>& albedo);

private:
	std::shared_ptr<Texture> m_albedo;
};

}// end namespace ph