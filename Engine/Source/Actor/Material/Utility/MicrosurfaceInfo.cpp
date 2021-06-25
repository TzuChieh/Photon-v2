#include "Actor/Material/Utility/MicrosurfaceInfo.h"
#include "Common/Logger.h"
#include "Common/assertion.h"
#include "Actor/Material/Utility/RoughnessToAlphaMapping.h"
#include "Core/SurfaceBehavior/Property/IsoTrowbridgeReitzConstant.h"
#include "Core/SurfaceBehavior/Property/IsoBeckmann.h"
#include "Core/SurfaceBehavior/Property/AnisoTrowbridgeReitz.h"

namespace ph
{

namespace
{
	const Logger logger(LogSender("Microsurface Info"));
}

MicrosurfaceInfo::MicrosurfaceInfo() : 
	m_type  (EType::TROWBRIDGE_REITZ),
	m_alphaU(RoughnessToAlphaMapping::squared(0.5_r)),
	m_alphaV(RoughnessToAlphaMapping::squared(0.5_r))
{}

std::unique_ptr<Microfacet> MicrosurfaceInfo::genMicrofacet() const
{
	if(isIsotropic())
	{
		PH_ASSERT_EQ(m_alphaU, m_alphaV);

		switch(m_type)
		{
		case EType::TROWBRIDGE_REITZ:
			return std::make_unique<IsoTrowbridgeReitzConstant>(m_alphaU);

		case EType::BECKMANN:
			return std::make_unique<IsoBeckmann>(m_alphaU);

		default:
			PH_ASSERT_UNREACHABLE_SECTION();
			return nullptr;
		}
	}
	else
	{
		if(m_type == EType::BECKMANN)
		{
			logger.log(ELogLevel::WARNING_MED,
				"anisotropic Beckmann is not supported; "
				"resort to Trowbridge-Reitz (GGX)");
		}

		return std::make_unique<AnisoTrowbridgeReitz>(m_alphaU, m_alphaV);
	}
}

}// end namespace ph
