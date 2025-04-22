#pragma once

#include "Engine/Core/VolumeBehavior/Property/MediumCoefficient.h"
#include "Engine/Core/SurfaceHit.h"
#include "Engine/Math/Color/Spectrum.h"

#include <Common/assertion.h>

#include <cmath>
#include <string>

namespace ph
{

class ConstantMediumCoefficient : public MediumCoefficient
{
public:
	inline ConstantMediumCoefficient()
		: ConstantMediumCoefficient(math::Spectrum(0))
	{}
	
	inline explicit ConstantMediumCoefficient(const math::Spectrum& sigmaA)
		: MediumCoefficient()
		, m_sigmaA(sigmaA)
	{}

	inline math::Spectrum eval(const SurfaceHit& /* X */) const override
	{
		return m_sigmaA;
	}

	inline math::Spectrum calcTransmittance(const real dist) const
	{
		PH_ASSERT_MSG(dist >= 0.0_r, 
			"dist = " + std::to_string(dist));

		math::Spectrum transmittance;
		for(std::size_t i = 0; i < math::Spectrum::NUM_VALUES; ++i)
		{
			transmittance[i] = std::exp(-m_sigmaA[i] * dist);
		}
		return transmittance;
	}

private:
	math::Spectrum m_sigmaA;
};

}// end namespace ph