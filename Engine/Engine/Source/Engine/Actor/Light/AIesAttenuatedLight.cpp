#include "Engine/Actor/Light/AIesAttenuatedLight.h"
#include "Engine/DataIO/Data/IesData.h"
#include "Engine/World/Foundation/PreCookReport.h"
#include "Engine/World/Foundation/CookingContext.h"
#include "Engine/World/Foundation/CookedResourceCollection.h"
#include "Engine/World/Foundation/TransientResourceCache.h"
#include "Engine/Math/constant.h"
#include "Engine/Frame/TFrame.h"
#include "Engine/Core/Emitter/TOmniModulatedEmitter.h"
#include "Engine/Core/Texture/Pixel/TFrameBuffer2D.h"
#include "Engine/Core/Texture/Pixel/TScalarPixelTexture2D.h"
#include "Engine/Core/Texture/Function/unary_texture_operators.h"
#include "Engine/Core/Intersection/PrimitiveMetadata.h"
#include "Engine/Core/Intersection/TMetaInjectionPrimitive.h"

#include <Common/assertion.h>
#include <Common/logging.h>
#include <Common/utility.h>

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
	TransientVisualElement sourceElement = getSourceVisualElement(ctx);
	if(sourceElement.surfaceEmitters.empty())
	{
		PH_LOG(ActorCooking, Warning,
			"ignoring this IES light: no emitters were found");
		return {};
	}

	if(!sourceElement.primitivesView.empty() &&
	   sourceElement.surfaceEmitters.size() != 1 &&
	   sourceElement.surfaceEmitters.size() != sourceElement.primitivesView.size())
	{
		PH_LOG(ActorCooking, Warning,
			"ignoring this IES light: no match between emitters and primitives "
			"(# emitters: {}, # primitives: {})",
			sourceElement.surfaceEmitters.size(), sourceElement.primitivesView.size());
		return {};
	}

	TransientVisualElement result;

	// Modulate source emitters with IES profile
	std::shared_ptr<TTexture<math::Spectrum>> attenuationTexture = loadAttenuationTexture();
	for(auto* sourceEmitter : sourceElement.surfaceEmitters)
	{
		auto* attenuatedEmitter = ctx.getResources()->makeEmitter<TOmniModulatedEmitter<SurfaceEmitter>>(sourceEmitter);
		attenuatedEmitter->setFilter(attenuationTexture);
		result.surfaceEmitters.push_back(attenuatedEmitter);
	}

	// Update source primitives with the modulated emitters
	if(!sourceElement.primitivesView.empty())
	{
		const PrimitiveMetadata* metadata = sourceElement.primitivesView[0]->getMetadata();
		PH_ASSERT(metadata);

		bool hasSingleMetadata = true;
		for(auto* sourcePrimitive : sourceElement.primitivesView)
		{
			if(sourcePrimitive->getMetadata() != metadata)
			{
				hasSingleMetadata = false;
				break;
			}
		}

		// This case can be implemented in the future.
		if(!hasSingleMetadata)
		{
			PH_NOT_IMPLEMENTED_WARNING();
			return {};
		}

		// 1 emitter to many primitives
		if(result.surfaceEmitters.size() == 1)
		{
			auto* iesMetadata = ctx.getResources()->makeMetadata(*metadata);
			iesMetadata->surface().setEmitter(result.surfaceEmitters[0]);
			for(auto* sourcePrimitive : sourceElement.primitivesView)
			{
				auto* iesPrimitive = ctx.getResources()->copyIntersectable(
					TMetaInjectionPrimitive(
						ReferencedPrimitiveMetaGetter(iesMetadata),
						TReferencedPrimitiveGetter<Primitive>(sourcePrimitive)));

				result.add(iesPrimitive);
			}
		}
		// 1 emitter to 1 primitive, with N pairs (N > 1)
		else
		{
			PH_ASSERT_EQ(result.surfaceEmitters.size(), sourceElement.primitivesView.size());
			for(std::size_t i = 0; i < result.surfaceEmitters.size(); ++i)
			{
				auto* iesMetadata = ctx.getResources()->makeMetadata(*metadata);
				iesMetadata->surface().setEmitter(result.surfaceEmitters[i]);

				auto* iesPrimitive = ctx.getResources()->copyIntersectable(
					TMetaInjectionPrimitive(
						ReferencedPrimitiveMetaGetter(iesMetadata),
						TReferencedPrimitiveGetter<Primitive>(sourceElement.primitivesView[i])));

				result.add(iesPrimitive);
			}
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

TransientVisualElement AIesAttenuatedLight::getSourceVisualElement(const CookingContext& ctx) const
{
	const TransientVisualElement* sourceElement = ctx.getCached(m_source);
	if(sourceElement)
	{
		return *sourceElement;
	}

	// Cook new visual element if not cached
	return m_source->stagelessCook(ctx);
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
