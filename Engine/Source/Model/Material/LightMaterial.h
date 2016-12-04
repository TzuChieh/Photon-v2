#pragma once

#include "Common/primitive_type.h"
#include "Model/Material/Material.h"
#include "Model/Material/Integrand/SiLight.h"

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

	void setEmittedRadiance(const Vector3f& radiance);
	void setEmittedRadiance(const float32 r, const float32 g, const float32 b);

private:
	SiLight m_surfaceIntegrand;
};

}// end namespace ph