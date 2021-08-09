#include "Actor/Material/Utility/DielectricInterfaceInfo.h"
#include "Core/SurfaceBehavior/Property/SchlickApproxDielectricFresnel.h"
#include "Core/SurfaceBehavior/Property/ExactDielectricFresnel.h"
#include "Common/logging.h"

namespace ph
{

PH_DEFINE_INTERNAL_LOG_GROUP(DielectricInterfaceInfo, BSDF);

std::unique_ptr<DielectricFresnel> DielectricInterfaceInfo::genFresnelEffect() const
{
	if(m_fresnel == EInterfaceFresnel::EXACT)
	{
		return std::make_unique<ExactDielectricFresnel>(
			m_iorOuter,
			m_iorInner);
	}
	else
	{
		PH_ASSERT(m_fresnel == EInterfaceFresnel::SCHLICK);

		return std::make_unique<SchlickApproxDielectricFresnel>(
			m_iorOuter,
			m_iorInner);
	}
}

}// end namespace ph
