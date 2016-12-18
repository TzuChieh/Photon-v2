#pragma once

#include "Common/primitive_type.h"

namespace ph
{

class Vector3f;

class Emitter
{
public:
	virtual ~Emitter() = 0;

	virtual void sampleDirect(const Vector3f& location, float32* const out_PDF, Vector3f* const out_emittedRadiance) const = 0;
};

}// end namespace ph