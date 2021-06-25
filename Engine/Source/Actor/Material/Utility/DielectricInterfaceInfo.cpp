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

DielectricInterfaceInfo::DielectricInterfaceInfo() :
	m_useExact(true),
	m_iorOuter(1.0_r),
	m_iorInner(1.5_r)
{}

std::unique_ptr<DielectricFresnel> DielectricInterfaceInfo::genFresnelEffect() const
{
	if(m_useExact)
	{
		return std::make_unique<ExactDielectricFresnel>(
			m_iorOuter,
			m_iorInner);
	}
	else
	{
		return std::make_unique<SchlickApproxDielectricFresnel>(
			m_iorOuter,
			m_iorInner);
	}
}

}// end namespace ph
