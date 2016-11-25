#pragma once

#include "Image/Texture.h"

namespace ph
{

class ConstantTexture : public Texture
{
public:
	ConstantTexture(const float32 value);
	virtual ~ConstantTexture() override;

	virtual void sample(const float32 u, const float32 v, Vector3f* const out_value) const override;

private:
	float32 m_value;
};

}// end namespace ph