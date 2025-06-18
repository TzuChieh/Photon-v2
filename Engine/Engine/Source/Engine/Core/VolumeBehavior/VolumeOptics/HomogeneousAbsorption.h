#pragma once

#include "Engine/Core/VolumeBehavior/VolumeOptics.h"
#include "Engine/Math/Color/Spectrum.h"

#include <memory>

namespace ph
{

class TransmittanceFunction;

class HomogeneousAbsorption : public VolumeOptics
{
public:
	HomogeneousAbsorption(
		const math::Spectrum& sigmaA,
		std::shared_ptr<TransmittanceFunction> transmittance);

	void genDistanceSampleCore(
		const MediumDistanceSampleInput& in,
		SampleFlow& sampleFlow,
		MediumDistanceSampleOutput& out) const override;

private:
	math::Spectrum m_sigmaA;
	std::shared_ptr<TransmittanceFunction> m_transmittance;
};

}// end namespace ph
