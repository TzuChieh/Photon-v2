#include "Actor/Texture/ConstantTexture.h"
#include "Math/Vector3f.h"

namespace ph
{

ConstantTexture::ConstantTexture(const Vector3f& value) :
	m_value(value)
{

}

ConstantTexture::ConstantTexture(const float32 r, const float32 g, const float32 b) :
	ConstantTexture(Vector3f(r, g, b))
{

}

ConstantTexture::~ConstantTexture() = default;

void ConstantTexture::sample(const Vector3f& uvw, Vector3f* const out_value) const
{
	out_value->set(m_value);
}

}// end namespace ph