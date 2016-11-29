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

	virtual void sample(const Vector3f& uvw, Vector3f* const out_value) const override;

private:
	Vector3f m_value;
};

}// end namespace ph