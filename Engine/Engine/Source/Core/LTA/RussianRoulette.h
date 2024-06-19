#pragma once

#include "Math/Color/Spectrum.h"
#include "Math/math.h"
#include "Core/SampleGenerator/SampleFlow.h"

#include <Common/assertion.h>

namespace ph::lta
{

class RussianRoulette final
{
public:
	bool surviveOnLuminance(
		const math::Spectrum& s,
		SampleFlow&           sampleFlow,
		math::Spectrum* const out_weightedS) const
	{
		PH_ASSERT(out_weightedS);

		// Survive rate is not allowed to be 100% to avoid immortal rays (e.g., TIR);
		// the rate should also kill non-finite luminance as this is a good place to do it
		const real rrSurviveRate = math::safe_clamp(s.relativeLuminance(), 0.0_r, 0.95_r);
		const real rrSpin        = sampleFlow.flow1D();// FIXME: use something like sampleFlow.binaryPick()

		// Survived
		if(rrSpin < rrSurviveRate)
		{
			PH_ASSERT(0.0_r < rrSurviveRate && rrSurviveRate <= 1.0_r);

			const real rrScale = 1.0_r / rrSurviveRate;
			*out_weightedS = s * rrScale;
			return true;
		}
		// Dead
		else
		{
			return false;
		}
	}
};

}// end namespace ph::lta
