#pragma once

#include "Model/Material/Material.h"
#include "Model/Material/LambertianDiffuseSurfaceIntegrand.h"
#include "Math/Vector3f.h"
#include "Image/ConstantTexture.h"

#include <memory>

namespace ph
{

class MatteOpaque : public Material
{
public:
	MatteOpaque();
	virtual ~MatteOpaque() override;

	virtual inline const SurfaceIntegrand* getSurfaceIntegrand() const override
	{
		return &m_surfaceIntegrand;
	}

	inline void getAlbedo(const Vector3f& uvw, Vector3f* const out_albedo) const
	{
		m_albedo->sample(uvw, out_albedo);
	}

	inline void setAlbedo(const float32 r, const float32 g, const float32 b)
	{
		m_albedo = std::make_shared<ConstantTexture>(Vector3f(r, g, b));
	}

	inline void setAlbedo(const std::shared_ptr<Texture>& albedo)
	{
		m_albedo = albedo;
	}

private:
	LambertianDiffuseSurfaceIntegrand m_surfaceIntegrand;
	std::shared_ptr<Texture> m_albedo;
};

}// end namespace ph