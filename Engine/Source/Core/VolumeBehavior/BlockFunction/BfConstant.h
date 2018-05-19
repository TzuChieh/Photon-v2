#pragma once

#include "Core/VolumeBehavior/BlockFunction.h"
#include "Core/SurfaceHit.h"

namespace ph
{

class BfConstant final : public BlockFunction
{
public:
	virtual inline ~BfConstant() override = default;

	virtual inline void evalAbsorptionCoeff(
		const SurfaceHit& X, 
		real* const       out_coeff) const override
	{
		// TODO
	}
};

}// end namespace ph