#include "Core/Estimator/PathEstimator.h"
#include "Core/Estimator/Estimation.h"
#include "Core/Quantity/SpectralStrength.h"
#include "Core/SurfaceHit.h"

namespace ph
{

PathEstimator::PathEstimator() = default;

AttributeTags PathEstimator::supportedAttributes() const
{
	AttributeTags supports;
	supports.tag(EAttribute::LIGHT_ENERGY);
	supports.tag(m_surfaceAttributeEstimator.supportedAttributes());
	return supports;
}

void PathEstimator::update(const Scene& scene)
{}

void PathEstimator::estimate(
	const Ray&           ray,
	const Integrand&     integrand,
	const AttributeTags& requestedAttributes,
	Estimation&          out_estimation) const
{
	if(requestedAttributes.isTagged(EAttribute::LIGHT_ENERGY))
	{
		SpectralStrength radiance;
		SurfaceHit       firstHit;
		radianceAlongRay(ray, integrand, radiance, firstHit);

		out_estimation.set<EAttribute::LIGHT_ENERGY>(radiance);

		m_surfaceAttributeEstimator.estimate(firstHit, requestedAttributes, out_estimation);
	}
	else
	{
		m_surfaceAttributeEstimator.estimate(ray, integrand, requestedAttributes, out_estimation);
	}
}

// command interface

PathEstimator::PathEstimator(const InputPacket& packet) : 
	Estimator(packet)
{}

SdlTypeInfo PathEstimator::ciTypeInfo()
{
	return SdlTypeInfo(ETypeCategory::REF_ESTIMATOR, "path");
}

void PathEstimator::ciRegister(CommandRegister& cmdRegister)
{}

}// end namespace ph