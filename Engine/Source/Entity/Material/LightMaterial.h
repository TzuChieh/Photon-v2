#pragma once

#include "Common/primitive_type.h"
#include "Entity/Material/Material.h"
#include "Entity/Material/SurfaceBehavior/SLight.h"

namespace ph
{

class LightMaterial : public Material
{
public:
	virtual ~LightMaterial() override;

	virtual const SurfaceBehavior* getSurfaceBehavior() const override
	{
		return &m_surfaceBehavior;
	}

	void setEmittedRadiance(const Vector3f& radiance);
	void setEmittedRadiance(const float32 r, const float32 g, const float32 b);

private:
	SLight m_surfaceBehavior;
};

}// end namespace ph