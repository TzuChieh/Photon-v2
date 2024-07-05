#include "Actor/Light/APointLight.h"
#include "Actor/Geometry/GSphere.h"

#include <Common/primitive_type.h>
#include <Common/logging.h>

namespace ph
{

namespace
{

constexpr real POINT_SOURCE_RADIUS = 0.005_r;

}

std::shared_ptr<Geometry> APointLight::getArea(const CookingContext& ctx) const
{
	auto sphere = TSdl<GSphere>::makeResource();
	sphere->setRadius(POINT_SOURCE_RADIUS);
	return sphere;
}

EmitterFeatureSet APointLight::getEmitterFeatureSet() const
{
	// Just use base implementation, but emit a note for potential high variance on certain settings
	const EmitterFeatureSet featureSet = AAreaLight::getEmitterFeatureSet();
	if(featureSet.has(EEmitterFeatureSet::BsdfSample))
	{
		PH_DEFAULT_DEBUG_LOG_ONCE(
			"BSDF sampling technique is enabled on a point light; please note that as point lights "
			"can easily produce extreme amount of energy density (e.g., radiance), this technique "
			"may produce high variance. However, depending on the rendering algorithm, disabling this "
			"technique can cause bias.");
	}

	return featureSet;
}

}// end namespace ph
