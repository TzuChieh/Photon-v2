#include "Engine/Core/VolumeBehavior/VolumeOptics/HomogeneousAbsorption.h"
#include "Engine/Core/VolumeBehavior/MediumDistanceSampleQuery.h"
#include "Engine/Core/VolumeBehavior/Property/TransmittanceFunction.h"

#include <Common/assertion.h>

#include <utility>

namespace ph
{

HomogeneousAbsorption::HomogeneousAbsorption(
	const math::Spectrum& sigmaA,
	std::shared_ptr<TransmittanceFunction> transmittance)

	: VolumeOptics()

	, m_sigmaA(sigmaA)
	, m_transmittance(std::move(transmittance))
{
	PH_ASSERT(m_transmittance);
}

void HomogeneousAbsorption::genDistanceSample(
	const MediumDistanceSampleInput& in,
	SampleFlow& sampleFlow,
	MediumDistanceSampleOutput& out) const
{
	out.setDist(in.getMaxDist());

	// PDF = 1
	out.setPdfAppliedWeight(m_transmittance->eval(m_sigmaA * in.getMaxDist()));
}

}// end namespace ph
