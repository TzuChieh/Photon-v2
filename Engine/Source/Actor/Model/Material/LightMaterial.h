#pragma once

#include "Common/primitive_type.h"
#include "Actor/Model/Material/Material.h"
#include "Core/SurfaceBehavior/SLight.h"

namespace ph
{

class LightMaterial : public Material
{
public:
	LightMaterial();
	virtual ~LightMaterial() override;

	virtual void populateSurfaceBehavior(SurfaceBehavior* const out_surfaceBehavior) const override;

	void setEmittedRadiance(const Vector3f& radiance);
	void setEmittedRadiance(const float32 r, const float32 g, const float32 b);

private:
	SLight m_bsdfCos;
};

}// end namespace ph