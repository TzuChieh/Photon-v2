#pragma once

#include "Model/Material/Material.h"
#include "Model/Material/LambertianDiffuseSurfaceIntegrand.h"
#include "Math/Vector3f.h"

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

	inline void getAlbedo(Vector3f* const out_albedo) const
	{
		out_albedo->set(m_albedo);
	}

	inline void setAlbedo(const float32 r, const float32 g, const float32 b)
	{
		m_albedo.set(r, g, b);
	}

private:
	LambertianDiffuseSurfaceIntegrand m_surfaceIntegrand;
	Vector3f m_albedo;
};

}// end namespace ph