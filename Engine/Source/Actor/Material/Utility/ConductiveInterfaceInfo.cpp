#include "Actor/Material/Utility/ConductiveInterfaceInfo.h"
#include "FileIO/SDL/InputPacket.h"
#include "FileIO/SDL/InputPrototype.h"
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
	m_ior     (SchlickIor{SpectralStrength(0.04_r)})
{}

ConductiveInterfaceInfo::ConductiveInterfaceInfo(const InputPacket& packet) :
	ConductiveInterfaceInfo()
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

	InputPrototype fullInput;
	fullInput.addReal("ior-outer");
	fullInput.addRealArray("ior-inner-wavelength-nm");
	fullInput.addRealArray("ior-inner-n");
	fullInput.addRealArray("ior-inner-k");

	// FIXME: f0's color space
	InputPrototype schlickInput;
	schlickInput.addVector3("f0");

	if(packet.isPrototypeMatched(fullInput))
	{
		const auto  iorOuter         = packet.getReal("ior-outer");
		const auto& iorWavelengthsNm = packet.getRealArray("ior-wavelengths-nm");
		const auto& iorInnerNs       = packet.getRealArray("ior-inner-n");
		const auto& iorInnerKs       = packet.getRealArray("ior-inner-k");

		if(iorWavelengthsNm.size() != iorInnerNs.size() ||
		   iorWavelengthsNm.size() != iorInnerKs.size())
		{
			logger.log(ELogLevel::WARNING_MED,
				"irregular-sized input data detected; " + 
				std::to_string(iorWavelengthsNm.size()) + " wavelengths given, with " + 
				std::to_string(iorInnerNs.size()) + " IoR_Ns and " + 
				std::to_string(iorInnerKs.size()) + " IoR_Ks");
			return;
		}

		const auto& sampledInnerNs = SpectralData::calcPiecewiseAveraged(
			iorWavelengthsNm.data(), iorInnerNs.data(), iorWavelengthsNm.size());
		const auto& sampledInnerKs = SpectralData::calcPiecewiseAveraged(
			iorWavelengthsNm.data(), iorInnerKs.data(), iorWavelengthsNm.size());

		SpectralStrength iorInnerN, iorInnerK;
		iorInnerN.setSampled(sampledInnerNs);
		iorInnerK.setSampled(sampledInnerKs);

		m_ior = FullIor{iorOuter, iorInnerN, iorInnerK};
	}
	else if(packet.isPrototypeMatched(schlickInput))
	{
		const auto f0 = packet.getVector3("f0");

		SpectralStrength spectralF0;
		spectralF0.setLinearSrgb(f0);// FIXME: check color space

		m_ior = SchlickIor{spectralF0};
	}
	else
	{
		logger.log(ELogLevel::WARNING_MED, 
			"no matching parameters found, using defaults");
	}
}

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
