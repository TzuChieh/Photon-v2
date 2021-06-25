#include "Actor/Material/Utility/ConductiveInterfaceInfo.h"
#include "Core/SurfaceBehavior/Property/SchlickApproxConductorFresnel.h"
#include "Core/SurfaceBehavior/Property/ExactConductorFresnel.h"
#include "Common/Logger.h"
#include "Core/Quantity/SpectralData.h"

namespace ph
{

namespace
{

const Logger logger(LogSender("Conductive Interface Info"));

}

ConductiveInterfaceInfo::ConductiveInterfaceInfo() :
	m_useExact(false),
	m_ior     (SchlickIor{Spectrum(0.04_r)})
{}

std::unique_ptr<ConductorFresnel> ConductiveInterfaceInfo::genFresnelEffect() const
{
	if(m_useExact && std::holds_alternative<FullIor>(m_ior))
	{
		const auto ior = std::get<FullIor>(m_ior);

		return std::make_unique<ExactConductorFresnel>(
			ior.outer,
			ior.innerN,
			ior.innerK);
	}
	else if(!m_useExact && std::holds_alternative<FullIor>(m_ior))
	{
		const auto ior = std::get<FullIor>(m_ior);

		return std::make_unique<SchlickApproxConductorFresnel>(
			ior.outer,
			ior.innerN,
			ior.innerK);
	}
	else if(!m_useExact && std::holds_alternative<SchlickIor>(m_ior))
	{
		const auto ior = std::get<SchlickIor>(m_ior);

		return std::make_unique<SchlickApproxConductorFresnel>(ior.f0);
	}
	else
	{
		logger.log(ELogLevel::WARNING_MED,
			"cannot generate fresnel effect; please check the input");

		return nullptr;
	}
}

}// end namespace ph
