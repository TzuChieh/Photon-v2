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
	virtual float32 calcImportanceSamplePdfW(const SurfaceSample& sample) const override;
	virtual void evaluate(SurfaceSample& sample) const override;

	void setAlbedo(const std::shared_ptr<Texture>& albedo);

private:
	std::shared_ptr<Texture> m_albedo;
};

}// end namespace ph