#pragma once

#include "Image/Texture.h"
#include "Math/Vector3f.h"

namespace ph
{

class ConstantTexture : public Texture
{
public:
	ConstantTexture(const Vector3f& value);
	virtual ~ConstantTexture() override;

	virtual void sample(const float32 u, const float32 v, Vector3f* const out_value) const override;

private:
	Vector3f m_value;
};

}// end namespace ph