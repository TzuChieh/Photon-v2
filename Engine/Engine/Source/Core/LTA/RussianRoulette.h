#pragma once

#include "Math/Color/Spectrum.h"
#include "Math/math.h"
#include "Core/SampleGenerator/SampleFlow.h"

#include <Common/assertion.h>
#include <Common/primitive_type.h>

namespace ph::lta
{

/*! @brief Russian roulette random termination scheme.
*/
class RussianRoulette final
{
public:
	/*!
	@param s The spectrum to play russian roulette. The survival rate is higher with higher luminance.
	@param sampleFlow The source of randomness.
	@return Whether the input spectrum `s` survived. If `false` is returned, the spectrum "died"
	and all output parameters are not usable.
	*/
	bool surviveOnLuminance(
		const math::Spectrum& s,
		SampleFlow& sampleFlow,
		real* out_survivalProbability) const;
};

inline bool RussianRoulette::surviveOnLuminance(
	const math::Spectrum& s,
	SampleFlow& sampleFlow,
	real* const out_survivalProbability) const
{
	PH_ASSERT(out_survivalProbability);

	// Survival rate is not allowed to be 100% to avoid immortal rays (e.g., TIR);
	// the rate should also kill non-finite luminance as this is a good place to do it
	const real rrSurvivalRate = math::safe_clamp(s.relativeLuminance(), 0.0_r, 0.95_r);
	const real rrSpin = sampleFlow.flow1D();// FIXME: use something like sampleFlow.binaryPick()

	// Survived
	if(rrSpin < rrSurvivalRate)
	{
		PH_ASSERT(0.0_r < rrSurvivalRate && rrSurvivalRate <= 1.0_r);

		*out_survivalProbability = rrSurvivalRate;
		return true;
	}
	// Dead
	else
	{
		return false;
	}
}

}// end namespace ph::lta
