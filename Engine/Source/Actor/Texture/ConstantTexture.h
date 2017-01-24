#pragma once

#include "Actor/Texture/Texture.h"
#include "Math/Vector3f.h"

namespace ph
{

class ConstantTexture : public Texture
{
public:
	ConstantTexture(const Vector3f& value);
	ConstantTexture(const float32 r, const float32 g, const float32 b);
	virtual ~ConstantTexture() override;

	virtual void sample(const Vector3f& uvw, Vector3f* const out_value) const override;

private:
	Vector3f m_value;
};

}// end namespace ph