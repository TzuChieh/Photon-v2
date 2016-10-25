#pragma once

#include "Common/primitive_type.h"

namespace ph
{

class Vector3f;

class RayIntegrand
{
public:
	virtual ~RayIntegrand() = 0;

	virtual void value(const Vector3f& rayInDir, const Vector3f& rayOutDir, const Vector3f& rayRadianceIn, Vector3f* const out_rayRadianceOut) const = 0;
};

}// end namespace ph