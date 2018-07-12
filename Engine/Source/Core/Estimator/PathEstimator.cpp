#include "Core/Estimator/PathEstimator.h"
#include "Core/Estimator/Estimation.h"
#include "Core/Quantity/SpectralStrength.h"
#include "Core/SurfaceHit.h"

namespace ph
{

PathEstimator::PathEstimator() = default;

PathEstimator::~PathEstimator() = default;

AttributeTags PathEstimator::supportedAttributes() const
{
	AttributeTags supports;
	supports.tag(EAttribute::LIGHT_ENERGY);
	return supports;
}

void PathEstimator::update(const Scene& scene)
{}

void PathEstimator::estimate(const Ray& ray, const Integrand& integrand, Estimation& estimation) const
{
	SpectralStrength radiance;
	SurfaceHit       firstHit;
	radianceAlongRay(ray, integrand, radiance, firstHit);

	estimation.set<EAttribute::LIGHT_ENERGY>(radiance);

	// TODO: other attributes from first hit
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