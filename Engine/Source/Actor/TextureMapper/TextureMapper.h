#pragma once

#include "Common/primitive_type.h"
#include "Math/math_fwd.h"

namespace ph
{

class TextureMapper
{
public:
	virtual ~TextureMapper() = 0;

	virtual void map(const Vector3R& position, const Vector3R& uvw, Vector3R* const out_uvw) const = 0;
};

}// end namespace ph