#include "Engine/World/SceneGlobals.h"
#include "Engine/Core/VolumeBehavior/Property/ExponentialTransmittance.h"

namespace ph
{

const ExponentialTransmittance* SceneGlobals::getExponentialTransmittance()
{
	static ExponentialTransmittance transmittance{};
	return &transmittance;
}

}// end namespace ph
