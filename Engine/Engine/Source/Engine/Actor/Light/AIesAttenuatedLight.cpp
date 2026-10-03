#include "Engine/Actor/Light/AIesAttenuatedLight.h"
#include "Engine/Actor/Basic/exceptions.h"
#include "Engine/DataIO/Data/IesData.h"
#include "Engine/World/Foundation/PreCookReport.h"
#include "Engine/World/Foundation/CookingContext.h"
#include "Engine/World/Foundation/CookedResourceCollection.h"
#include "Engine/Math/constant.h"
#include "Engine/Frame/TFrame.h"
#include "Engine/Core/Emitter/TOmniModulatedEmitter.h"
#include "Engine/Core/Texture/Pixel/TFrameBuffer2D.h"
#include "Engine/Core/Texture/Pixel/TScalarPixelTexture2D.h"
#include "Engine/Core/Texture/Function/unary_texture_operators.h"
#include "Engine/Core/Intersection/PrimitiveBuilder.h"
#include "Engine/Core/Intersection/PrimitiveMetadata.h"

#include <Common/assertion.h>
#include <Common/logging.h>
#include <Common/utility.h>

#include <algorithm>
#include <cstddef>
#include <unordered_map>

namespace ph
{

PreCookReport AIesAttenuatedLight::preCook(const CookingContext& ctx) const
{
	PreCookReport report = ALight::preCook(ctx);
	if(!report.isCookable())
	{
		return report;
	}

	if(!m_source)
	{
		PH_LOG(ActorCooking, Warning,
			"ignoring this IES light: light source is not specified");
		report.markAsUncookable();
	}

	if(!m_iesFile.isResolved())
	{
		PH_LOG(ActorCooking, Warning,
			"ignoring this IES light: IES file is missing");
		report.markAsUncookable();
	}

	return report;
}

TransientVisualElement AIesAttenuatedLight::cook(
	const CookingContext& ctx, const PreCookReport& report) const
{
	const TransientVisualElement* sourceResult = ctx.getCached(m_source);
	if(!sourceResult)
	{
		throw ActorCookException(
			"IES light source dependency was not cooked and cached");
	}
	const TransientVisualElement& sourceElement = *sourceResult;

	if(sourceElement.surfaceEmitters.empty())
	{
		PH_LOG(ActorCooking, Warning,
			"ignoring IES attenuation: no emitters were found");
		return sourceElement;
	}

	TransientVisualElement result = sourceElement;

	// Modulate source emitters with IES profile
	const std::shared_ptr<TTexture<math::Spectrum>> attenuationTexture = loadAttenuationTexture();
	for(auto& emitterUnit : result.surfaceEmitters)
	{
		auto* attenuatedEmitter = ctx.getResources().makeEmitter<TOmniModulatedEmitter<SurfaceEmitter>>(
			emitterUnit.emitter);
		attenuatedEmitter->setFilter(attenuationTexture);
		emitterUnit.emitter = attenuatedEmitter;
	}

	// Update source primitives with the modulated emitters
	std::unordered_map<const PrimitiveMetadata*, PrimitiveMetadata*> sourceMetadataToIesMetadata;
	std::unordered_map<const Intersectable*, const Primitive*> sourcePrimitiveToIesPrimitive;
	for(const Primitive*& primitive : result.primitivesView)
	{
		bool isEmissive = false;
		for(uint32 slot = 0; slot < primitive->numMetadataSlots(); ++slot)
		{
			if(primitive->getMetadata(slot).getSurface().isEmissive())
			{
				isEmissive = true;
				break;
			}
		}
		if(!isEmissive)
		{
			continue;
		}

		// Wrapping emissive primitives with multiple metadata slots can be implemented in the future.
		if(primitive->numMetadataSlots() != 1)
		{
			PH_NOT_IMPLEMENTED_WARNING();
			return {};
		}

		// Primitives sharing source metadata also share the decorated metadata.
		const PrimitiveMetadata& sourceMetadata = primitive->getMetadata(0);
		PrimitiveMetadata*& iesMetadata = sourceMetadataToIesMetadata[&sourceMetadata];
		if(!iesMetadata)
		{
			// Find source emitter's index so we can access the wrapped one.
			const auto emitterIter = std::ranges::find(
				sourceElement.surfaceEmitters,
				&sourceMetadata.getSurface().getEmitter(),
				&TransientVisualElement::SurfaceEmitterUnit::emitter);
			if(emitterIter == sourceElement.surfaceEmitters.end())
			{
				throw ActorCookException(
					"IES light source primitive references an unregistered emitter");
			}

			const auto emitterIndex = static_cast<std::size_t>(
				emitterIter - sourceElement.surfaceEmitters.begin());
			iesMetadata = ctx.getResources().makeMetadata(sourceMetadata);
			iesMetadata->surface().setEmitter(result.surfaceEmitters[emitterIndex].emitter);
		}

		const Primitive*& iesPrimitive = sourcePrimitiveToIesPrimitive[primitive];
		if(!iesPrimitive)
		{
			iesPrimitive = ctx.getResources().copyIntersectable(
				PrimitiveBuilder::referencing(primitive)
					.injectMetadata(iesMetadata)
					.build());
		}
		primitive = iesPrimitive;
	}

	// Update entity lists to reference the IES wrappers already in `primitivesView`.
	for(const Intersectable*& intersectable : result.intersectables)
	{
		const auto replacementIter = sourcePrimitiveToIesPrimitive.find(intersectable);
		if(replacementIter != sourcePrimitiveToIesPrimitive.end())
		{
			intersectable = replacementIter->second;
		}
	}
	for(const Primitive*& primitive : result.nonBlockingEmitterPrimitives)
	{
		const auto replacementIter = sourcePrimitiveToIesPrimitive.find(primitive);
		if(replacementIter != sourcePrimitiveToIesPrimitive.end())
		{
			primitive = replacementIter->second;
		}
	}

	return result;
}

void AIesAttenuatedLight::setSource(const std::shared_ptr<ALight>& source)
{
	m_source = source;
}

void AIesAttenuatedLight::setIesFile(const Path& iesFile)
{
	m_iesFile.setPath(iesFile);
}

std::shared_ptr<TTexture<math::Spectrum>> AIesAttenuatedLight::loadAttenuationTexture() const
{
	std::shared_ptr<TTexture<math::Spectrum>> attenuationTexture;
	{
		const IesData iesData(m_iesFile.getPath());
	
		const uint32 pixelsPerDegree = 2;
		const uint32 widthPx         = 360 * pixelsPerDegree;
		const uint32 heightPx        = 180 * pixelsPerDegree;

		TFrame<real, 1> attenuationFactors(widthPx, heightPx);
		for(uint32 y = 0; y < heightPx; y++)
		{
			for(uint32 x = 0; x < widthPx; x++)
			{
				const real u     = (static_cast<real>(x) + 0.5_r) / static_cast<real>(widthPx);
				const real v     = (static_cast<real>(y) + 0.5_r) / static_cast<real>(heightPx);
				const real phi   = u * math::constant::two_pi<real>;
				const real theta = (1.0_r - v) * math::constant::pi<real>;

				const real factor = iesData.sampleAttenuationFactor(theta, phi);
				attenuationFactors.setPixel(x, y, TFrame<real, 1>::PixelType(factor));
			}
		}

		auto attenuationFactorTexture = std::make_shared<TScalarPixelTexture2D<real>>(
			std::make_shared<TFrameBuffer2D<real, 1>>(attenuationFactors),
			0);
		
		// Convert sampled scalar to spectrum
		auto factorToSpectrumTexture = std::make_shared<TUnaryTextureOperator<
			math::TArithmeticArray<real, 1>, 
			math::Spectrum, 
			texfunc::TScalarToSpectrum<real>>>(attenuationFactorTexture);

		attenuationTexture = factorToSpectrumTexture;
	}

	return attenuationTexture;
}

}// end namespace ph
