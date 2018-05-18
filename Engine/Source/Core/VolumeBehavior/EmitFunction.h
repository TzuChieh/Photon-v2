#pragma once

#include "Common/primitive_type.h"

namespace ph
{

class SurfaceHit;

class EmitFunction
{
public:
	virtual ~EmitFunction() = 0;

	virtual void evalEmissionCoeff(const SurfaceHit& X, real* out_coeff) const = 0;
};

}// end namespace ph