#include "Actor/LightSource/IesAttenuatedSource.h"
#include "DataIO/Data/IesData.h"
#include "Core/Intersectable/UvwMapper/SphericalMapper.h"
#include "Core/Intersectable/PrimitiveMetadata.h"
#include "Core/Intersectable/PrimitiveChannel.h"
#include "Frame/TFrame.h"
#include "Core/Texture/TBilinearPixelTex2D.h"
#include "Core/Texture/Function/TConversionTexture.h"
#include "Math/constant.h"
#include "Core/Emitter/OmniModulatedEmitter.h"
#include "Common/logging.h"

namespace ph
{

PH_DEFINE_INTERNAL_LOG_GROUP(IESAttenuatedSource, Light);

IesAttenuatedSource::IesAttenuatedSource() : 
	IesAttenuatedSource(nullptr, Path())
{}

IesAttenuatedSource::IesAttenuatedSource(
	const std::shared_ptr<LightSource>& source, 
	const Path&                         iesFile) : 

	LightSource()
{
	setSource(source);
	setIesFile(iesFile);
}

std::unique_ptr<Emitter> IesAttenuatedSource::genEmitter(
	ActorCookingContext& ctx, EmitterBuildingMaterial&& data) const
{
	if(!m_source)
	{
		PH_LOG_WARNING(IESAttenuatedSource,
			"no light source specified, ignoring");
		return nullptr;
	}

	const IesData iesData(m_iesFile);
	
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
			attenuationFactors.setPixel(x, y, TFrame<real, 1>::Pixel(factor));
		}
	}

	auto attenuationTexture = std::make_shared<TBilinearPixelTex2D<real, 1>>(attenuationFactors);
	auto sourceEmitter      = m_source->genEmitter(ctx, std::move(data));
	auto attenuatedEmitter  = std::make_unique<OmniModulatedEmitter>(std::move(sourceEmitter));
	auto convertedTexture   = std::make_shared<TConversionTexture<TTexPixel<real, 1>, Spectrum>>(attenuationTexture);

	attenuatedEmitter->setFilter(convertedTexture);

	return attenuatedEmitter;
}

std::shared_ptr<Geometry> IesAttenuatedSource::genGeometry(ActorCookingContext& ctx) const
{
	if(!m_source)
	{
		return nullptr;
	}

	return m_source->genGeometry(ctx);
}

std::shared_ptr<Material> IesAttenuatedSource::genMaterial(ActorCookingContext& ctx) const
{
	if(!m_source)
	{
		return nullptr;
	}

	return m_source->genMaterial(ctx);
}

}// end namespace ph
