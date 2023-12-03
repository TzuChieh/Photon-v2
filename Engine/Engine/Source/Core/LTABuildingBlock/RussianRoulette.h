#pragma once

#include "Common/assertion.h"
#include "Math/Color/Spectrum.h"
#include "Math/math.h"
#include "Core/SampleGenerator/SampleFlow.h"

namespace ph
{

class RussianRoulette final
{
public:
	static bool surviveOnLuminance(
		const math::Spectrum& s,
		SampleFlow&           sampleFlow,
		math::Spectrum* const out_weightedS)
	{
		PH_ASSERT(out_weightedS);

		// survive rate is not allowed to be 100% to avoid immortal rays (e.g., TIR)
		const real rrSurviveRate = math::clamp(s.relativeLuminance(), 0.0_r, 0.95_r);
		const real rrSpin        = sampleFlow.flow1D();// FIXME: use something like sampleFlow.binaryPick()

		// survived
		if(rrSpin < rrSurviveRate)
		{
			PH_ASSERT(0.0_r < rrSurviveRate && rrSurviveRate <= 1.0_r);

			const real rrScale = 1.0_r / rrSurviveRate;
			*out_weightedS = s.mul(rrScale);
			return true;
		}
		// dead
		else
		{
			return false;
		}
	}
};

}// end namespace ph
