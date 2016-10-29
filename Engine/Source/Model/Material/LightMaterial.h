#pragma once

#include "Common/primitive_type.h"
#include "Model/Material/Material.h"
#include "Model/Material/LightSurfaceIntegrand.h"

namespace ph
{

class LightMaterial : public Material
{
public:
	virtual ~LightMaterial() override;

	virtual const SurfaceIntegrand* getSurfaceIntegrand() const override
	{
		return &m_surfaceIntegrand;
	}

	inline void setEmittedRadiance(const float32 r, const float32 g, const float32 b)
	{
		m_surfaceIntegrand.setEmittedRadiance(r, g, b);
	}

private:
	LightSurfaceIntegrand m_surfaceIntegrand;
};

}// end namespace ph