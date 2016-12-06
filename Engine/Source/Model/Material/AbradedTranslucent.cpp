#include "Model/Material/AbradedTranslucent.h"
#include "Image/ConstantTexture.h"

#include <memory>

namespace ph
{

AbradedTranslucent::AbradedTranslucent() :
	m_surfaceIntegrand()
{

}

AbradedTranslucent::~AbradedTranslucent() = default;

void AbradedTranslucent::setF0(const Vector3f& f0)
{
	setF0(f0.x, f0.y, f0.z);
}

void AbradedTranslucent::setF0(const float32 r, const float32 g, const float32 b)
{
	m_surfaceIntegrand.setF0(std::make_shared<ConstantTexture>(r, g, b));
}

void AbradedTranslucent::setIOR(const float32 ior)
{
	m_surfaceIntegrand.setIOR(std::make_shared<ConstantTexture>(ior, ior, ior));
}

void AbradedTranslucent::setRoughness(const float32 roughness)
{
	m_surfaceIntegrand.setRoughness(std::make_shared<ConstantTexture>(roughness, roughness, roughness));
}

}// end namespace ph