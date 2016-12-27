#include "Actor/Model/Material/LightMaterial.h"

namespace ph
{

LightMaterial::LightMaterial() : 
	m_bsdfCos()
{
	
}

LightMaterial::~LightMaterial() = default;

void LightMaterial::populateSurfaceBehavior(SurfaceBehavior* const out_surfaceBehavior) const
{
	out_surfaceBehavior->setBsdfCos(std::make_unique<SLight>(m_bsdfCos));
}

void LightMaterial::setEmittedRadiance(const Vector3f& radiance)
{
	setEmittedRadiance(radiance.x, radiance.y, radiance.z);
}

void LightMaterial::setEmittedRadiance(const float32 r, const float32 g, const float32 b)
{
	m_bsdfCos.setEmittedRadiance(r, g, b);
}

}// end namespace ph