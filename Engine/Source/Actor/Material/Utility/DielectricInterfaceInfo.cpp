#include "Actor/Material/Utility/DielectricInterfaceInfo.h"
#include "Core/SurfaceBehavior/Property/SchlickApproxDielectricFresnel.h"
#include "Core/SurfaceBehavior/Property/ExactDielectricFresnel.h"
#include "Common/Logger.h"

namespace ph
{

namespace
{

const Logger logger(LogSender("Dielectric Interface Info"));

}

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
