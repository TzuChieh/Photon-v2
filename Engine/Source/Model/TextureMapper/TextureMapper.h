#pragma once

#include "Common/primitive_type.h"

namespace ph
{

class Vector3f;

class TextureMapper
{
public:
	virtual ~TextureMapper() = 0;

	virtual void map(const Vector3f& position, const Vector3f& uvw, Vector3f* const out_uvw) const = 0;
};

}// end namespace ph