#include "Model/Material/AbradedOpaque.h"
#include "Image/ConstantTexture.h"

#include <memory>

namespace ph
{

AbradedOpaque::AbradedOpaque() : 
	m_surfaceIntegrand()
{

}

AbradedOpaque::~AbradedOpaque() = default;

void AbradedOpaque::setRoughness(const float32 roughness)
{
	m_surfaceIntegrand.setRoughness(std::make_shared<ConstantTexture>(roughness, roughness, roughness));
}

void AbradedOpaque::setF0(const Vector3f& f0)
{
	m_surfaceIntegrand.setF0(std::make_shared<ConstantTexture>(f0));
}

}// end namespace ph