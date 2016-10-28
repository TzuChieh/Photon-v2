#pragma once

#include "Model/Material/Material.h"
#include "Model/Material/LambertianDiffuseSurfaceIntegrand.h"

namespace ph
{

class MatteOpaque : public Material
{
public:
	MatteOpaque();
	virtual ~MatteOpaque() override;

	virtual inline const SurfaceIntegrand* getSurfaceIntegrand() override
	{
		return &m_surfaceIntegrand;
	}

private:
	LambertianDiffuseSurfaceIntegrand m_surfaceIntegrand;
};

}// end namespace ph