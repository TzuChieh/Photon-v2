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

#include <string>
#include <iostream>

namespace ph
{

IdealSubstance::IdealSubstance() : 
	SurfaceMaterial(),
	m_opticsGenerator()
{
	asAbsorber();
}

void IdealSubstance::genSurface(ActorCookingContext& ctx, SurfaceBehavior& behavior) const
{
	PH_ASSERT(m_opticsGenerator);

	behavior.setOptics(m_opticsGenerator(ctx));
}

void IdealSubstance::asDielectricReflector(const real iorInner, const real iorOuter)
{
	m_opticsGenerator = [=](ActorCookingContext& ctx)
	{
		auto fresnel = std::make_shared<ExactDielectricFresnel>(iorOuter, iorInner);
		auto optics  = std::make_unique<IdealReflector>(fresnel);
		return optics;
	};
}

void IdealSubstance::asMetallicReflector(const math::Vector3R& linearSrgbF0, const real iorOuter)
{
	Spectrum f0Spectral;
	f0Spectral.setLinearSrgb(linearSrgbF0);// FIXME: check color space

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
			auto reflectionScale = ConstantImage(linearSrgbReflectionScale, ConstantImage::EType::RAW_LINEAR_SRGB);
			auto transmissionScale = ConstantImage(linearSrgbTransmissionScale, ConstantImage::EType::RAW_LINEAR_SRGB);

			return std::make_unique<IdealDielectric>(
				fresnel, 
				reflectionScale.genTextureSpectral(ctx), 
				transmissionScale.genTextureSpectral(ctx));
		}
	};
}

}// end namespace ph
