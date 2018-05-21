#pragma once

#include "Common/primitive_type.h"
#include "Core/Quantity/SpectralStrength.h"

namespace ph
{

class SurfaceHit;

class BlockFunction
{
public:
	virtual ~BlockFunction() = 0;

	virtual void evalAbsorptionCoeff(const SurfaceHit& X, SpectralStrength* out_coeff) const = 0;
};

}// end namespace ph