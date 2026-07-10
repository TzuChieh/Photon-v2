#include "Engine/Actor/Material/IdealSubstance.h"
#include "Engine/Actor/Basic/exceptions.h"
#include "Engine/Actor/Material/Component/ConductiveInterfaceInfo.h"
#include "Engine/Actor/Material/Component/DielectricInterfaceInfo.h"
#include "Engine/Core/SurfaceBehavior/Property/surface_property.h"
#include "Engine/Core/SurfaceBehavior/SurfaceOptics/IdealAbsorber.h"
#include "Engine/Core/SurfaceBehavior/SurfaceOptics/TIdealDielectric.h"
#include "Engine/Core/SurfaceBehavior/SurfaceOptics/TIdealDielectricTransmitter.h"
#include "Engine/Core/SurfaceBehavior/SurfaceOptics/TIdealReflector.h"
#include "Engine/World/Foundation/CookedMaterial.h"
#include "Engine/World/Foundation/CookedResourceCollection.h"
#include "Engine/World/Foundation/CookingContext.h"

#include <Common/assertion.h>
#include <Common/logging.h>

#include <string>
#include <utility>

namespace ph
{

PH_DEFINE_INTERNAL_LOG_GROUP(IdealSubstance, Material);

void IdealSubstance::storeCooked(
	const CookingContext& ctx,
	CookedMaterial& out_material) const
{
	using ConstantScale = TConstantSurfaceProperty<math::Spectrum>;
	using TexturedScale = TTexturedSurfaceProperty<math::Spectrum>;

	switch(m_substance)
	{
	case EIdealSubstance::Absorber:
		out_material.surfaceOptics = ctx.getResources().makeSurfaceOptics<IdealAbsorber>();
		break;

	case EIdealSubstance::DielectricReflector:
	{
		auto interfaceInfo = DielectricInterfaceInfo(m_fresnel, m_iorOuter, m_iorInner);

		if(m_reflectionScaleMap)
		{
			using Optics = TIdealReflector<TexturedScale>;

			out_material.surfaceOptics = ctx.getResources().makeSurfaceOptics<Optics>(
				interfaceInfo.genFresnelEffect(ctx),
				TexturedScale(m_reflectionScaleMap->genColorTexture(ctx)));
		}
		else
		{
			using Optics = TIdealReflector<ConstantScale>;

			if(m_reflectionScale == math::Spectrum(1))
			{
				out_material.surfaceOptics = ctx.getResources().makeSurfaceOptics<Optics>(
					interfaceInfo.genFresnelEffect(ctx));
			}
			else
			{
				out_material.surfaceOptics = ctx.getResources().makeSurfaceOptics<Optics>(
					interfaceInfo.genFresnelEffect(ctx),
					ConstantScale(m_reflectionScale));
			}
		}
	}
	break;

	case EIdealSubstance::Dielectric:
	{
		auto interfaceInfo = DielectricInterfaceInfo(m_fresnel, m_iorOuter, m_iorInner);
		auto fresnel = interfaceInfo.genFresnelEffect(ctx);

		if(m_reflectionScaleMap && m_transmissionScaleMap)
		{
			using Optics = TIdealDielectric<TexturedScale, TexturedScale>;

			out_material.surfaceOptics = ctx.getResources().makeSurfaceOptics<Optics>(
				std::move(fresnel),
				TexturedScale(m_reflectionScaleMap->genColorTexture(ctx)),
				TexturedScale(m_transmissionScaleMap->genColorTexture(ctx)));
		}
		else if(m_reflectionScaleMap)
		{
			using Optics = TIdealDielectric<TexturedScale, ConstantScale>;

			out_material.surfaceOptics = ctx.getResources().makeSurfaceOptics<Optics>(
				std::move(fresnel),
				TexturedScale(m_reflectionScaleMap->genColorTexture(ctx)),
				ConstantScale(m_transmissionScale));
		}
		else if(m_transmissionScaleMap)
		{
			using Optics = TIdealDielectric<ConstantScale, TexturedScale>;

			out_material.surfaceOptics = ctx.getResources().makeSurfaceOptics<Optics>(
				std::move(fresnel),
				ConstantScale(m_reflectionScale),
				TexturedScale(m_transmissionScaleMap->genColorTexture(ctx)));
		}
		else
		{
			using Optics = TIdealDielectric<ConstantScale, ConstantScale>;

			if(m_reflectionScale == math::Spectrum(1) &&
			   m_transmissionScale == math::Spectrum(1))
			{
				out_material.surfaceOptics = ctx.getResources().makeSurfaceOptics<Optics>(
					std::move(fresnel));
			}
			else
			{
				out_material.surfaceOptics = ctx.getResources().makeSurfaceOptics<Optics>(
					std::move(fresnel),
					ConstantScale(m_reflectionScale),
					ConstantScale(m_transmissionScale));
			}
		}
	}
	break;

	case EIdealSubstance::MetallicReflector:
	{
		auto interfaceInfo = ConductiveInterfaceInfo();
		interfaceInfo.setFresnel(m_fresnel);
		interfaceInfo.setF0(m_f0);
		if(m_f0Map)
		{
			interfaceInfo.setF0Map(m_f0Map);
		}
		interfaceInfo.setIorOuter(m_iorOuter);

		if(m_iorInnerN)
		{
			interfaceInfo.setIorInnerN(*m_iorInnerN);
		}

		if(m_iorInnerK)
		{
			interfaceInfo.setIorInnerK(*m_iorInnerK);
		}

		if(m_reflectionScaleMap)
		{
			using Optics = TIdealReflector<TexturedScale>;

			out_material.surfaceOptics = ctx.getResources().makeSurfaceOptics<Optics>(
				interfaceInfo.genFresnelEffect(ctx),
				TexturedScale(m_reflectionScaleMap->genColorTexture(ctx)));
		}
		else
		{
			using Optics = TIdealReflector<ConstantScale>;

			if(m_reflectionScale == math::Spectrum(1))
			{
				out_material.surfaceOptics = ctx.getResources().makeSurfaceOptics<Optics>(
					interfaceInfo.genFresnelEffect(ctx));
			}
			else
			{
				out_material.surfaceOptics = ctx.getResources().makeSurfaceOptics<Optics>(
					interfaceInfo.genFresnelEffect(ctx),
					ConstantScale(m_reflectionScale));
			}
		}
	}
	break;

	case EIdealSubstance::DielectricTransmitter:
	{
		auto interfaceInfo = DielectricInterfaceInfo(m_fresnel, m_iorOuter, m_iorInner);
		auto fresnel = interfaceInfo.genFresnelEffect(ctx);

		if(m_transmissionScaleMap)
		{
			using Optics = TIdealDielectricTransmitter<TexturedScale>;

			out_material.surfaceOptics = ctx.getResources().makeSurfaceOptics<Optics>(
				std::move(fresnel),
				TexturedScale(m_transmissionScaleMap->genColorTexture(ctx)));
		}
		else
		{
			using Optics = TIdealDielectricTransmitter<ConstantScale>;

			if(m_transmissionScale == math::Spectrum(1))
			{
				out_material.surfaceOptics = ctx.getResources().makeSurfaceOptics<Optics>(
					std::move(fresnel));
			}
			else
			{
				out_material.surfaceOptics = ctx.getResources().makeSurfaceOptics<Optics>(
					std::move(fresnel),
					ConstantScale(m_transmissionScale));
			}
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

void IdealSubstance::setF0Map(std::shared_ptr<Image> f0Map)
{
	m_f0Map = std::move(f0Map);
}

void IdealSubstance::setReflectionScaleMap(std::shared_ptr<Image> reflectionScaleMap)
{
	m_reflectionScaleMap = std::move(reflectionScaleMap);
}

void IdealSubstance::setTransmissionScaleMap(std::shared_ptr<Image> transmissionScaleMap)
{
	m_transmissionScaleMap = std::move(transmissionScaleMap);
}

}// end namespace ph
