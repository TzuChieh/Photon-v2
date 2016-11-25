#pragma once

#include "Common/primitive_type.h"

namespace ph
{

class Vector3f;

class TextureMapper
{
public:
	virtual ~TextureMapper() = 0;

	virtual void map(const Vector3f& position, const float32 u, const float32 v, float32* const out_u, float32* const out_v) const = 0;
};

}// end namespace ph