#include "Engine/Actor/Material/Volume/IdealMedium.h"
#include "Engine/Core/VolumeBehavior/VolumeOptics/HomogeneousAbsorption.h"
#include "Engine/Core/VolumeBehavior/Property/ExponentialTransmittance.h"
#include "Engine/World/Foundation/CookedMaterial.h"
#include "Engine/World/Foundation/CookingContext.h"
#include "Engine/World/Foundation/CookedResourceCollection.h"
#include "Engine/World/SceneGlobals.h"

namespace ph
{

void IdealMedium::storeCooked(
	CookedMaterial& out_material,
	const CookingContext& ctx) const
{
	out_material.volumeCompositions.push_back({
		.optics = ctx.getResources().makeVolumeOptics<HomogeneousAbsorption>(
			m_absorptionCoeff,
			SceneGlobals::getExponentialTransmittance()),
		.type = EVolumeComposition::General});
}

}// end namespace ph
