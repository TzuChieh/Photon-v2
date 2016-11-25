#pragma once

#include "Common/primitive_type.h"

namespace ph
{

class Vector3f;

class Texture
{
public:
	virtual ~Texture() = 0;

	virtual void sample(const float32 u, const float32 v, Vector3f* const out_value) const = 0;
};

}// end namespace ph