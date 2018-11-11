#pragma once

#include "Common/assertion.h"
#include "Core/Quantity/SpectralStrength.h"
#include "Math/Random.h"
#include "Math/math.h"

namespace ph
{

class RussianRoulette final
{
public:
	static bool surviveOnLuminance(
		const SpectralStrength& s, 
		SpectralStrength* const out_weightedS)
	{
		PH_ASSERT(out_weightedS);

		// survive rate is not allowed to be 100% to avoid immortal rays (e.g., TIR)
		const real rrSurviveRate = math::clamp(s.calcLuminance(), 0.0_r, 0.95_r);
		const real rrSpin        = Random::genUniformReal_i0_e1();

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