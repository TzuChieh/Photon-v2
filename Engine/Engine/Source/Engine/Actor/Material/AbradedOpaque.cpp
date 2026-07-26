#include "Engine/Actor/Material/AbradedOpaque.h"
#include "Engine/Core/SurfaceBehavior/Property/surface_property.h"
#include "Engine/Core/SurfaceBehavior/SurfaceOptics/TIdealReflector.h"
#include "Engine/Core/SurfaceBehavior/SurfaceOptics/TOpaqueMicrofacet.h"
#include "Engine/World/Foundation/CookedMaterial.h"
#include "Engine/World/Foundation/CookingContext.h"
#include "Engine/World/Foundation/CookedResourceCollection.h"

#include <utility>

namespace ph
{

void AbradedOpaque::storeCooked(
	const CookingContext& ctx,
	CookedMaterial& out_material) const
{
	using ConstantScale = TConstantSurfaceProperty<math::Spectrum>;
	using TexturedScale = TTexturedSurfaceProperty<math::Spectrum>;

	auto fresnel = m_interfaceInfo.genFresnelEffect(ctx);

	// Equivalent to ideal surface
	if(m_microsurfaceInfo.isPerfectlySmooth())
	{
		if(m_reflectionScaleMap)
		{
			using Optics = TIdealReflector<TexturedScale>;

			out_material.surfaceOptics = ctx.getResources().makeSurfaceOptics<Optics>(
				std::move(fresnel),
				TexturedScale(m_reflectionScaleMap->genColorTexture(ctx)));
		}
		else
		{
			using Optics = TIdealReflector<ConstantScale>;

			if(m_reflectionScale == math::Spectrum(1))
			{
				out_material.surfaceOptics = ctx.getResources().makeSurfaceOptics<Optics>(
					std::move(fresnel));
			}
			else
			{
				out_material.surfaceOptics = ctx.getResources().makeSurfaceOptics<Optics>(
					std::move(fresnel),
					ConstantScale(m_reflectionScale));
			}
		}

		return;
	}

	auto microfacet = m_microsurfaceInfo.genMicrofacet(ctx);

	if(m_reflectionScaleMap)
	{
		using Optics = TOpaqueMicrofacet<TexturedScale>;

		out_material.surfaceOptics = ctx.getResources().makeSurfaceOptics<Optics>(
			std::move(fresnel),
			std::move(microfacet),
			TexturedScale(m_reflectionScaleMap->genColorTexture(ctx)));
	}
	else if(m_reflectionScale == math::Spectrum(1))
	{
		out_material.surfaceOptics = ctx.getResources().makeSurfaceOptics<OpaqueMicrofacet>(
			std::move(fresnel),
			std::move(microfacet));
	}
	else
	{
		using Optics = TOpaqueMicrofacet<ConstantScale>;

		out_material.surfaceOptics = ctx.getResources().makeSurfaceOptics<Optics>(
			std::move(fresnel),
			std::move(microfacet),
			ConstantScale(m_reflectionScale));
	}
}

void AbradedOpaque::setReflectionScaleMap(std::shared_ptr<Image> reflectionScaleMap)
{
	m_reflectionScaleMap = std::move(reflectionScaleMap);
}

}// end namespace ph
