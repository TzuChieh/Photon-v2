#pragma once

#include "Entity/Material/SurfaceBehavior/SurfaceBehavior.h"
#include "Math/Vector3f.h"
#include "Image/Texture.h"
#include "Image/ConstantTexture.h"

#include <memory>

namespace ph
{

class SLambertianDiffuse : public SurfaceBehavior
{
public:
	SLambertianDiffuse();
	virtual ~SLambertianDiffuse() override;

	virtual void genBsdfCosImportanceSample(const Intersection& intersection, const Ray& ray, SurfaceSample* const out_sample) const override;

	void setAlbedo(const std::shared_ptr<Texture>& albedo);

private:
	std::shared_ptr<Texture> m_albedo;
};

}// end namespace ph