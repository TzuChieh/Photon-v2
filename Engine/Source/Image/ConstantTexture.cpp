#include "Image/ConstantTexture.h"
#include "Math/Vector3f.h"

namespace ph
{

ConstantTexture::ConstantTexture(const Vector3f& value) :
	m_value(value)
{

}

ConstantTexture::~ConstantTexture() = default;

void ConstantTexture::sample(const float32 u, const float32 v, Vector3f* const out_value) const
{
	out_value->set(m_value);
}

}// end namespace ph