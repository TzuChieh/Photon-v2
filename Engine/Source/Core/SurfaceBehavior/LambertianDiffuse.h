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

	virtual void genImportanceSample(SurfaceSample& sample) const override;
	//virtual void genImportanceSample(const Intersection& intersection, const Ray& ray, SurfaceSample* const out_sample, float32* const out_pdfW) const override;
	virtual void evaluate(const Intersection& intersection, const Vector3f& L, const Vector3f& V, Vector3f* const out_value) const override;

	void setAlbedo(const std::shared_ptr<Texture>& albedo);

private:
	std::shared_ptr<Texture> m_albedo;
};

}// end namespace ph