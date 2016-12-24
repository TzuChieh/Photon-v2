#include "Entity/Material/MatteOpaque.h"
#include "Image/ConstantTexture.h"

namespace ph
{

MatteOpaque::MatteOpaque() : 
	m_surfaceBehavior()
{

}

MatteOpaque::~MatteOpaque() = default;

void MatteOpaque::setAlbedo(const Vector3f& albedo)
{
	setAlbedo(albedo.x, albedo.y, albedo.z);
}

void MatteOpaque::setAlbedo(const float32 r, const float32 g, const float32 b)
{
	setAlbedo(std::make_shared<ConstantTexture>(r, g, b));
}

void MatteOpaque::setAlbedo(const std::shared_ptr<Texture>& albedo)
{
	m_surfaceBehavior.setAlbedo(albedo);
}

}// end namespace ph