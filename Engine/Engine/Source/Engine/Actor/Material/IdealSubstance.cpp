#include "Engine/Actor/Material/IdealSubstance.h"
#include "Engine/Core/SurfaceBehavior/SurfaceOptics/IdealReflector.h"
#include "Engine/Core/SurfaceBehavior/SurfaceOptics/IdealDielectricTransmitter.h"
#include "Engine/Core/SurfaceBehavior/Property/ExactDielectricFresnel.h"
#include "Engine/Core/SurfaceBehavior/Property/SchlickApproxConductorFresnel.h"
#include "Engine/Math/TVector3.h"
#include "Engine/Core/SurfaceBehavior/SurfaceOptics/IdealAbsorber.h"
#include "Engine/Core/SurfaceBehavior/SurfaceBehavior.h"
#include "Engine/Core/SurfaceBehavior/SurfaceOptics/IdealDielectric.h"
#include "Engine/Core/Texture/constant_textures.h"
#include "Engine/Actor/Basic/exceptions.h"
#include "Engine/Actor/Material/Component/DielectricInterfaceInfo.h"
#include "Engine/Actor/Material/Component/ConductiveInterfaceInfo.h"

#include <Common/assertion.h>
#include <Common/logging.h>

#include <string>
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

void IdealSubstance::genSurface(const CookingContext& ctx, SurfaceBehavior& behavior) const
{
	switch(m_substance)
	{
	case EIdealSubstance::Absorber:
		behavior.setOptics(std::make_shared<IdealAbsorber>());
		break;

	case EIdealSubstance::DielectricReflector:
	{
		auto interfaceInfo = DielectricInterfaceInfo(m_fresnel, m_iorOuter, m_iorInner);

		if(m_reflectionScale == math::Spectrum(1))
		{
			behavior.setOptics(std::make_shared<IdealReflector>(interfaceInfo.genFresnelEffect()));
		}
		else
		{
			behavior.setOptics(std::make_shared<IdealReflector>(
				interfaceInfo.genFresnelEffect(),
				std::make_shared<TConstantTexture<math::Spectrum>>(m_reflectionScale)));
		}
	}
	break;

	case EIdealSubstance::Dielectric:
	{
		auto interfaceInfo = DielectricInterfaceInfo(m_fresnel, m_iorOuter, m_iorInner);
		auto fresnel       = interfaceInfo.genFresnelEffect();

		if(m_reflectionScale == math::Spectrum(1) && m_transmissionScale == math::Spectrum(1))
		{
			behavior.setOptics(std::make_shared<IdealDielectric>(std::move(fresnel)));
		}
		else
		{
			behavior.setOptics(std::make_shared<IdealDielectric>(
				std::move(fresnel),
				std::make_shared<TConstantTexture<math::Spectrum>>(m_reflectionScale),
				std::make_shared<TConstantTexture<math::Spectrum>>(m_transmissionScale)));
		}
	}
	break;

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

		if(m_reflectionScale == math::Spectrum(1))
		{
			behavior.setOptics(std::make_shared<IdealReflector>(interfaceInfo.genFresnelEffect()));
		}
		else
		{
			behavior.setOptics(std::make_shared<IdealReflector>(
				interfaceInfo.genFresnelEffect(),
				std::make_shared<TConstantTexture<math::Spectrum>>(m_reflectionScale)));
		}
	}
	break;

	case EIdealSubstance::DielectricTransmitter:
	{
		auto interfaceInfo = DielectricInterfaceInfo(m_fresnel, m_iorOuter, m_iorInner);
		auto fresnel       = interfaceInfo.genFresnelEffect();

		if(m_transmissionScale == math::Spectrum(1))
		{
			behavior.setOptics(std::make_shared<IdealDielectricTransmitter>(std::move(fresnel)));
		}
		else
		{
			behavior.setOptics(std::make_shared<IdealDielectricTransmitter>(
				std::move(fresnel),
				std::make_shared<TConstantTexture<math::Spectrum>>(m_transmissionScale)));
		}
	}
	break;

	default:
		throw CookException("Unsupported ideal substance type.");
	}
}

void IdealSubstance::setSubstance(const EIdealSubstance substance)
{
	m_substance = substance;
}

}// end namespace ph
