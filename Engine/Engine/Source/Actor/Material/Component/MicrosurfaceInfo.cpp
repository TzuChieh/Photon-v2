#include "Actor/Material/Component/MicrosurfaceInfo.h"
#include "Actor/Material/Component/RoughnessToAlphaMapping.h"
#include "Core/SurfaceBehavior/Property/IsoTrowbridgeReitzConstant.h"
#include "Core/SurfaceBehavior/Property/IsoBeckmann.h"
#include "Core/SurfaceBehavior/Property/AnisoTrowbridgeReitz.h"

#include <Common/logging.h>

namespace ph
{

PH_DEFINE_INTERNAL_LOG_GROUP(MicrosurfaceInfo, BSDF);

std::unique_ptr<Microfacet> MicrosurfaceInfo::genMicrofacet(
	const EInterfaceMicrosurface defaultType) const
{
	EInterfaceMicrosurface microsurfaceType = m_microsurface;
	if(microsurfaceType == EInterfaceMicrosurface::Unspecified)
	{
		microsurfaceType = defaultType;
	}

	if(isIsotropic())
	{
		const real alpha = RoughnessToAlphaMapping::map(getIsotropicRoughness(), m_roughnessToAlpha);

		switch(microsurfaceType)
		{
		case EInterfaceMicrosurface::TrowbridgeReitz:
			return std::make_unique<IsoTrowbridgeReitzConstant>(alpha, m_maskingShadowing);

		case EInterfaceMicrosurface::Beckmann:
			return std::make_unique<IsoBeckmann>(alpha, m_maskingShadowing);

		default:
			PH_LOG(MicrosurfaceInfo, Warning,
				"type of microsurface not specified; resort to Trowbridge-Reitz (GGX)");
			return std::make_unique<IsoTrowbridgeReitzConstant>(alpha, m_maskingShadowing);
		}
	}
	else
	{
		PH_ASSERT(!isIsotropic());

		if(microsurfaceType == EInterfaceMicrosurface::Beckmann)
		{
			PH_LOG(MicrosurfaceInfo, Warning,
				"anisotropic Beckmann is not supported; resort to Trowbridge-Reitz (GGX)");
		}

		const auto [roughnessU, roughnessV] = getAnisotropicUVRoughnesses();

		return std::make_unique<AnisoTrowbridgeReitz>(
			RoughnessToAlphaMapping::map(roughnessU, m_roughnessToAlpha), 
			RoughnessToAlphaMapping::map(roughnessV, m_roughnessToAlpha),
			m_maskingShadowing);
	}
}

}// end namespace ph
