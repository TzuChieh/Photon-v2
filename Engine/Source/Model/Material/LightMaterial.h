#pragma once

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

private:
	LightSurfaceIntegrand m_surfaceIntegrand;
};

}// end namespace ph