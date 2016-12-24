#include "Entity/Material/AbradedOpaque.h"
#include "Image/ConstantTexture.h"

#include <memory>

namespace ph
{

AbradedOpaque::AbradedOpaque() : 
	m_surfaceBehavior()
{

}

AbradedOpaque::~AbradedOpaque() = default;

void AbradedOpaque::setRoughness(const float32 roughness)
{
	m_surfaceBehavior.setRoughness(std::make_shared<ConstantTexture>(roughness, roughness, roughness));
}

void AbradedOpaque::setF0(const Vector3f& f0)
{
	setF0(f0.x, f0.y, f0.z);
}

void AbradedOpaque::setF0(const float32 r, const float32 g, const float32 b)
{
	m_surfaceBehavior.setF0(std::make_shared<ConstantTexture>(r, g, b));
}

}// end namespace ph