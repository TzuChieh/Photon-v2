#include "Engine/Actor/Material/AbradedTranslucent.h"
#include "Engine/Core/SurfaceBehavior/Property/surface_property.h"
#include "Engine/Core/SurfaceBehavior/SurfaceOptics/TIdealDielectric.h"
#include "Engine/Core/SurfaceBehavior/SurfaceOptics/TTranslucentMicrofacet.h"
#include "Engine/World/Foundation/CookedMaterial.h"
#include "Engine/World/Foundation/CookingContext.h"
#include "Engine/World/Foundation/CookedResourceCollection.h"

#include <utility>

namespace ph
{

void AbradedTranslucent::storeCooked(
	const CookingContext& ctx,
	CookedMaterial& out_material) const
{
	using ConstantScale = TConstantSurfaceProperty<math::Spectrum>;
	using TexturedScale = TTexturedSurfaceProperty<math::Spectrum>;

	auto fresnel = m_interfaceInfo.genFresnelEffect(ctx);

	// Equivalent to ideal surface
	if(m_microsurfaceInfo.isPerfectlySmooth())
	{
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

			if(m_reflectionScale == math::Spectrum(1) && m_transmissionScale == math::Spectrum(1))
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

		return;
	}

	auto microfacet = m_microsurfaceInfo.genMicrofacet(ctx);

	if(m_reflectionScaleMap && m_transmissionScaleMap)
	{
		using Optics = TTranslucentMicrofacet<TexturedScale, TexturedScale>;

		out_material.surfaceOptics = ctx.getResources().makeSurfaceOptics<Optics>(
			std::move(fresnel),
			std::move(microfacet),
			TexturedScale(m_reflectionScaleMap->genColorTexture(ctx)),
			TexturedScale(m_transmissionScaleMap->genColorTexture(ctx)));
	}
	else if(m_reflectionScaleMap)
	{
		using Optics = TTranslucentMicrofacet<TexturedScale, ConstantScale>;

		out_material.surfaceOptics = ctx.getResources().makeSurfaceOptics<Optics>(
			std::move(fresnel),
			std::move(microfacet),
			TexturedScale(m_reflectionScaleMap->genColorTexture(ctx)),
			ConstantScale(m_transmissionScale));
	}
	else if(m_transmissionScaleMap)
	{
		using Optics = TTranslucentMicrofacet<ConstantScale, TexturedScale>;

		out_material.surfaceOptics = ctx.getResources().makeSurfaceOptics<Optics>(
			std::move(fresnel),
			std::move(microfacet),
			ConstantScale(m_reflectionScale),
			TexturedScale(m_transmissionScaleMap->genColorTexture(ctx)));
	}
	else
	{
		using Optics = TTranslucentMicrofacet<ConstantScale, ConstantScale>;

		if(m_reflectionScale == math::Spectrum(1) && m_transmissionScale == math::Spectrum(1))
		{
			out_material.surfaceOptics = ctx.getResources().makeSurfaceOptics<Optics>(
				std::move(fresnel),
				std::move(microfacet));
		}
		else
		{
			out_material.surfaceOptics = ctx.getResources().makeSurfaceOptics<Optics>(
				std::move(fresnel),
				std::move(microfacet),
				ConstantScale(m_reflectionScale),
				ConstantScale(m_transmissionScale));
		}
	}
}

void AbradedTranslucent::setRoughness(const real roughness)
{
	m_microsurfaceInfo.setRoughness(roughness);
}

void AbradedTranslucent::setRoughnessMap(std::shared_ptr<Image> roughnessMap)
{
	m_microsurfaceInfo.setRoughnessMap(std::move(roughnessMap));
}

void AbradedTranslucent::setRoughnessVMap(std::shared_ptr<Image> roughnessVMap)
{
	m_microsurfaceInfo.setRoughnessVMap(std::move(roughnessVMap));
}

void AbradedTranslucent::setReflectionScaleMap(std::shared_ptr<Image> reflectionScaleMap)
{
	m_reflectionScaleMap = std::move(reflectionScaleMap);
}

void AbradedTranslucent::setTransmissionScaleMap(std::shared_ptr<Image> transmissionScaleMap)
{
	m_transmissionScaleMap = std::move(transmissionScaleMap);
}

}// end namespace ph
