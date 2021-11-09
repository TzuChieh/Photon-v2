#include "Actor/Material/IdealSubstance.h"
#include "Core/SurfaceBehavior/SurfaceOptics/IdealReflector.h"
#include "Core/SurfaceBehavior/SurfaceOptics/IdealTransmitter.h"
#include "Core/SurfaceBehavior/Property/ExactDielectricFresnel.h"
#include "Core/SurfaceBehavior/Property/SchlickApproxConductorFresnel.h"
#include "Math/TVector3.h"
#include "Core/SurfaceBehavior/SurfaceOptics/IdealAbsorber.h"
#include "Common/assertion.h"
#include "Core/SurfaceBehavior/SurfaceBehavior.h"
#include "Core/SurfaceBehavior/SurfaceOptics/IdealDielectric.h"
#include "Actor/Image/ConstantImage.h"
#include "Common/logging.h"
#include "Actor/actor_exceptions.h"
#include "Actor/Material/Utility/DielectricInterfaceInfo.h"
#include "Actor/Material/Utility/ConductiveInterfaceInfo.h"

#include <string>
#include <iostream>
#include <utility>

namespace ph
{

PH_DEFINE_INTERNAL_LOG_GROUP(IdealSubstance, Material);

IdealSubstance::IdealSubstance() : 

	SurfaceMaterial(),

	m_substance        (EIdealSubstance::Absorber),
	m_fresnel          (EInterfaceFresnel::Schlick),
	m_iorOuter         (1.0_r),
	m_iorInner         (1.5_r),
	m_f0               (1),
	m_reflectionScale  (1),
	m_transmissionScale(1),
	m_iorInnerN        (),
	m_iorInnerK        ()
{}

void IdealSubstance::genSurface(ActorCookingContext& ctx, SurfaceBehavior& behavior) const
{
	switch(m_substance)
	{
	case EIdealSubstance::Absorber:
		behavior.setOptics(std::make_shared<IdealAbsorber>());
		break;

	case EIdealSubstance::DielectricReflector:
	{
		// TODO: transmission scale
		auto interfaceInfo = DielectricInterfaceInfo(m_fresnel, m_iorOuter, m_iorInner);
		behavior.setOptics(std::make_shared<IdealReflector>(interfaceInfo.genFresnelEffect()));
		break;
	}

	case EIdealSubstance::MetallicReflector:
	{
		auto interfaceInfo = ConductiveInterfaceInfo();
		interfaceInfo.setFresnel(m_fresnel);
		interfaceInfo.setF0(m_f0);
		interfaceInfo.setIorOuter(m_iorOuter);

		if(m_iorInnerN)
		{
			interfaceInfo.setIorInnerN(*m_iorInnerN);
		}
		
		if(m_iorInnerK)
		{
			interfaceInfo.setIorInnerN(*m_iorInnerK);
		}

		behavior.setOptics(std::make_shared<IdealReflector>(interfaceInfo.genFresnelEffect()));
		break;
	}


	default:
		throw ActorCookException("Unsupported ideal substance type.");
	}

	
}


void IdealSubstance::asMetallicReflector(const math::Vector3R& linearSrgbF0, const real iorOuter)
{
	math::Spectrum f0Spectral;
	f0Spectral.setLinearSRGB(linearSrgbF0.toArray(), math::EColorUsage::ECF);// FIXME: check color space

	m_opticsGenerator = [=](ActorCookingContext& ctx)
	{
		auto fresnel = std::make_shared<SchlickApproxConductorFresnel>(f0Spectral);
		auto optics  = std::make_unique<IdealReflector>(fresnel);
		return optics;
	};
}

void IdealSubstance::asTransmitter(const real iorInner, const real iorOuter)
{
	m_opticsGenerator = [=](ActorCookingContext& ctx)
	{
		auto fresnel = std::make_shared<ExactDielectricFresnel>(iorOuter, iorInner);
		auto optics  = std::make_unique<IdealTransmitter>(fresnel);
		return optics;
	};
}

void IdealSubstance::asAbsorber()
{
	m_opticsGenerator = [=](ActorCookingContext& ctx)
	{
		return std::make_unique<IdealAbsorber>();
	};
}

void IdealSubstance::asDielectric(
	const real iorInner,
	const real iorOuter,
	const math::Vector3R& linearSrgbReflectionScale,
	const math::Vector3R& linearSrgbTransmissionScale)
{
	m_opticsGenerator = [=](ActorCookingContext& ctx)
	{
		auto fresnel = std::make_shared<ExactDielectricFresnel>(iorOuter, iorInner);

		if(linearSrgbReflectionScale == math::Vector3R(1.0_r) && linearSrgbTransmissionScale == math::Vector3R(1.0_r))
		{
			return std::make_unique<IdealDielectric>(fresnel);
		}
		else
		{
			auto reflectionScale = ConstantImage(linearSrgbReflectionScale, math::EColorSpace::Linear_sRGB);
			auto transmissionScale = ConstantImage(linearSrgbTransmissionScale, math::EColorSpace::Linear_sRGB);

			return std::make_unique<IdealDielectric>(
				fresnel, 
				reflectionScale.genColorTexture(ctx), 
				transmissionScale.genColorTexture(ctx));
		}
	};
}

}// end namespace ph
