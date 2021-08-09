#include "Actor/Material/Utility/ConductiveInterfaceInfo.h"
#include "Core/SurfaceBehavior/Property/SchlickApproxConductorFresnel.h"
#include "Core/SurfaceBehavior/Property/ExactConductorFresnel.h"
#include "Common/logging.h"

namespace ph
{

PH_DEFINE_INTERNAL_LOG_GROUP(ConductiveInterfaceInfo, BSDF);

std::unique_ptr<ConductorFresnel> ConductiveInterfaceInfo::genFresnelEffect() const
{
	// If we have enough information for an exact Fresnel description
	if(m_iorOuter && m_iorInnerN && m_iorInnerK)
	{
		if(m_fresnel == EInterfaceFresnel::EXACT)
		{
			return std::make_unique<ExactConductorFresnel>(
				*m_iorOuter,
				*m_iorInnerN,
				*m_iorInnerK);
		}
		else
		{
			// If Schlick approximation is explicitly requested, use exact
			// Fresnel description to derive required input.

			PH_ASSERT(m_fresnel == EInterfaceFresnel::SCHLICK);
		
			return std::make_unique<SchlickApproxConductorFresnel>(
				*m_iorOuter,
				*m_iorInnerN,
				*m_iorInnerK);
		}
	}
	else
	{
		if(m_fresnel == EInterfaceFresnel::EXACT)
		{
			// If exact Fresnel is explicitly requested, since we do not have
			// complete information for that, fallback to Schlick approximation
			// and issue a warning.

			PH_LOG_WARNING(ConductiveInterfaceInfo,
				"exact Fresnel formula is requested without complete information "
				"specified; will fallback to Schlick approximation");
		}

		return std::make_unique<SchlickApproxConductorFresnel>(m_f0);
	}
}

}// end namespace ph
