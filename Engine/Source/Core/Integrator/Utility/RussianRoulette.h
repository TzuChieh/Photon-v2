#pragma once

#include "Common/assertion.h"
#include "Core/Quantity/SpectralStrength.h"
#include "Math/Random.h"
#include "Math/Math.h"

namespace ph
{

class RussianRoulette final
{
public:
	static void surviveOnLuminance(
		const SpectralStrength& s, 
		SpectralStrength* const out_weightedS)
	{
		PH_ASSERT(out_weightedS);

		const real rrSurviveRate = Math::clamp(s.calcLuminance(), 0.0001_r, 1.0_r);
		const real rrSpin        = Random::genUniformReal_i0_e1();

		// russian roulette >> survive
		if(rrSurviveRate > rrSpin)
		{
			PH_ASSERT(0.0_r < rrSurviveRate && rrSurviveRate <= 1.0_r);

			const real rrScale = 1.0_r / rrSurviveRate;
			*out_weightedS = s.mul(rrScale);
		}
		// russian roulette >> dead
		else
		{
			out_weightedS->setValues(0.0_r);
		}
	}
};

}// end namespace ph