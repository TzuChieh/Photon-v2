#pragma once

#include "Common/primitive_type.h"

namespace ph
{

class SurfaceHit;

class BlockFunction final
{
public:
	virtual ~BlockFunction() = 0;

	virtual void evalAbsorptionCoeff(const SurfaceHit& X, real* out_coeff) const = 0;
};

}// end namespace ph