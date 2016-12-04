#include "Model/Material/LightMaterial.h"

namespace ph
{

LightMaterial::~LightMaterial() = default;

void LightMaterial::setEmittedRadiance(const Vector3f& radiance)
{
	setEmittedRadiance(radiance.x, radiance.y, radiance.z);
}

void LightMaterial::setEmittedRadiance(const float32 r, const float32 g, const float32 b)
{
	m_surfaceIntegrand.setEmittedRadiance(r, g, b);
}

}// end namespace ph