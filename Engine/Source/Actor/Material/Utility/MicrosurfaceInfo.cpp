#include "Actor/Material/Utility/MicrosurfaceInfo.h"
#include "FileIO/SDL/InputPacket.h"
#include "Common/Logger.h"
#include "Common/assertion.h"
#include "Actor/Material/Utility/RoughnessToAlphaMapping.h"
#include "Core/SurfaceBehavior/Property/IsoTrowbridgeReitz.h"
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

MicrosurfaceInfo::MicrosurfaceInfo(const InputPacket& packet) : 
	MicrosurfaceInfo()
{
	real roughnessU = 0.5_r, roughnessV = 0.5_r;
	if(packet.hasReal("roughness-u") && packet.hasReal("roughness-v"))
	{
		roughnessU = packet.getReal("roughness-u");
		roughnessV = packet.getReal("roughness-v");
	}
	else if(packet.hasReal("roughness"))
	{
		roughnessU = packet.getReal("roughness");
		roughnessV = roughnessU;
	}
	else
	{
		logger.log(ELogLevel::WARNING_MED, 
			"no roughness specified, using defaults");
	}

	const auto mappingType = packet.getString("mapping", "squared");
	if(mappingType == "squared")
	{
		m_alphaU = RoughnessToAlphaMapping::squared(roughnessU);
		m_alphaV = RoughnessToAlphaMapping::squared(roughnessV);
	}
	else if(mappingType == "pbrt-v3")
	{
		m_alphaU = RoughnessToAlphaMapping::pbrtV3(roughnessU);
		m_alphaV = RoughnessToAlphaMapping::pbrtV3(roughnessV);
	}
	else if(mappingType == "equaled")
	{
		m_alphaU = RoughnessToAlphaMapping::equaled(roughnessU);
		m_alphaV = RoughnessToAlphaMapping::equaled(roughnessV);
	}
	else
	{
		logger.log(ELogLevel::WARNING_MED,
			"unknown mapping type <" + mappingType + "> specified; resort to squared");

		m_alphaU = RoughnessToAlphaMapping::squared(roughnessU);
		m_alphaV = RoughnessToAlphaMapping::squared(roughnessV);
	}

	const auto microsurfaceType = packet.getString("microsurface");
	if(microsurfaceType == "trowbridge-reitz" || microsurfaceType == "ggx")
	{
		m_type = EType::TROWBRIDGE_REITZ;
	}
	else if(microsurfaceType == "beckmann")
	{
		m_type = EType::BECKMANN;
	}
	else
	{
		logger.log(ELogLevel::WARNING_MED,
			"unknown mapping type <" + mappingType + "> specified; resort to Trowbridge-Reitz (GGX)");

		m_type = EType::TROWBRIDGE_REITZ;
	}
}

std::unique_ptr<Microfacet> MicrosurfaceInfo::genMicrofacet() const
{
	if(isIsotropic())
	{
		PH_ASSERT_EQ(m_alphaU, m_alphaV);

		switch(m_type)
		{
		case EType::TROWBRIDGE_REITZ:
			return std::make_unique<IsoTrowbridgeReitz>(m_alphaU);

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
				"anisotropic Beckmann is not supported; resort to Trowbridge-Reitz (GGX)");
		}

		return std::make_unique<AnisoTrowbridgeReitz>(m_alphaU, m_alphaV);
	}
}

}// end namespace ph
