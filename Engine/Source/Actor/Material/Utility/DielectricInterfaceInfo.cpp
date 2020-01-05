#include "Actor/Material/Utility/DielectricInterfaceInfo.h"
#include "DataIO/SDL/InputPacket.h"
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

DielectricInterfaceInfo::DielectricInterfaceInfo(const InputPacket& packet) :
	DielectricInterfaceInfo()
{
	const auto fresnelType = packet.getString("fresnel-model", "schlick");
	if(fresnelType == "exact")
	{
		m_useExact = true;
	}
	else if(fresnelType == "schlick")
	{
		m_useExact = false;
	}
	else
	{
		logger.log(ELogLevel::WARNING_MED,
			"unknwon fresnel model: " + fresnelType + "; "
			"resort to schlick approximation");
		m_useExact = false;
	}

	m_iorOuter = packet.getReal("ior-outer", m_iorOuter);
	m_iorInner = packet.getReal("ior-inner", m_iorInner);
}

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
