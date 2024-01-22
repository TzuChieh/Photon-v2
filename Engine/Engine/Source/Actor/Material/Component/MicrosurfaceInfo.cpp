#include "Actor/Material/Component/MicrosurfaceInfo.h"
#include "Actor/Material/Component/RoughnessToAlphaMapping.h"
#include "Core/SurfaceBehavior/Property/IsoTrowbridgeReitzConstant.h"
#include "Core/SurfaceBehavior/Property/IsoBeckmann.h"
#include "Core/SurfaceBehavior/Property/AnisoTrowbridgeReitz.h"

#include <Common/logging.h>

namespace ph
{

PH_DEFINE_INTERNAL_LOG_GROUP(MicrosurfaceInfo, BSDF);

std::unique_ptr<Microfacet> MicrosurfaceInfo::genMicrofacet() const
{
	if(isIsotropic())
	{
		const real alpha = RoughnessToAlphaMapping::map(getIsotropicRoughness(), m_roughnessToAlpha);

		switch(m_microsurface)
		{
		case EInterfaceMicrosurface::TrowbridgeReitz:
			return std::make_unique<IsoTrowbridgeReitzConstant>(alpha);

		case EInterfaceMicrosurface::Beckmann:
			return std::make_unique<IsoBeckmann>(alpha);

		default:
			PH_LOG_WARNING(MicrosurfaceInfo, 
				"no input provided for the type of microsurface; resort to Trowbridge-Reitz (GGX)");
			return std::make_unique<IsoTrowbridgeReitzConstant>(alpha);
		}
	}
	else
	{
		PH_ASSERT(!isIsotropic());

		if(m_microsurface == EInterfaceMicrosurface::Beckmann)
		{
			PH_LOG_WARNING(MicrosurfaceInfo,
				"anisotropic Beckmann is not supported; resort to Trowbridge-Reitz (GGX)");
		}

		const auto [roughnessU, roughnessV] = getAnisotropicUVRoughnesses();

		return std::make_unique<AnisoTrowbridgeReitz>(
			RoughnessToAlphaMapping::map(roughnessU, m_roughnessToAlpha), 
			RoughnessToAlphaMapping::map(roughnessV, m_roughnessToAlpha));
	}
}

}// end namespace ph
