#pragma once

#include "Core/SurfaceBehavior/BSDFcos.h"
#include "Math/Vector3f.h"
#include "Image/Texture.h"
#include "Image/ConstantTexture.h"

#include <memory>

namespace ph
{

class LambertianDiffuse : public BSDFcos
{
public:
	LambertianDiffuse();
	virtual ~LambertianDiffuse() override;

	virtual void genImportanceSample(const Intersection& intersection, const Ray& ray, SurfaceSample* const out_sample) const override;
	virtual void evaluate(const Intersection& intersection, const Vector3f& wi, const Vector3f& wo, Vector3f* const out_value) const override;

	void setAlbedo(const std::shared_ptr<Texture>& albedo);

private:
	std::shared_ptr<Texture> m_albedo;
};

}// end namespace ph