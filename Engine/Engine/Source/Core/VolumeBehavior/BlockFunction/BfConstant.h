#pragma once

#include "Core/VolumeBehavior/BlockFunction.h"
#include "Core/SurfaceHit.h"
#include "Math/Color/Spectrum.h"

#include <Common/assertion.h>

#include <cmath>
#include <string>

namespace ph
{

class BfConstant final : public BlockFunction
{
public:
	inline BfConstant() :
		BfConstant(math::Spectrum(0))
	{}

	inline BfConstant(const math::Spectrum& coeff) :
		BlockFunction(),
		m_absorptionCoeff(coeff)
	{}

	virtual inline ~BfConstant() override = default;

	virtual inline void evalAbsorptionCoeff(
		const SurfaceHit&     /* X */, 
		math::Spectrum* const out_coeff) const override
	{
		PH_ASSERT(out_coeff);

		*out_coeff = m_absorptionCoeff;
	}

	inline math::Spectrum calcTransmittance(const real dist) const
	{
		PH_ASSERT_MSG(dist >= 0.0_r, 
			"dist = " + std::to_string(dist));

		math::Spectrum transmittance;
		for(std::size_t i = 0; i < math::Spectrum::NUM_VALUES; ++i)
		{
			transmittance[i] = std::exp(-m_absorptionCoeff[i] * dist);
		}
		return transmittance;
	}

private:
	math::Spectrum m_absorptionCoeff;
};

}// end namespace ph