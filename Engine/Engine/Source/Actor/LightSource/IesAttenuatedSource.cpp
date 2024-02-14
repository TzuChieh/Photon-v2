#include "Actor/LightSource/IesAttenuatedSource.h"
#include "DataIO/Data/IesData.h"
#include "Core/Intersection/PrimitiveMetadata.h"
#include "Core/Intersection/PrimitiveChannel.h"
#include "Frame/TFrame.h"
#include "Core/Texture/Pixel/TFrameBuffer2D.h"
#include "Core/Texture/Pixel/TScalarPixelTexture2D.h"
#include "Core/Texture/Function/unary_texture_operators.h"
#include "Math/constant.h"
#include "Core/Emitter/OmniModulatedEmitter.h"

#include <Common/logging.h>

#include <memory>

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
	CookingContext& ctx, EmitterBuildingMaterial&& data) const
{
	//if(!m_source)
	//{
	//	PH_LOG_WARNING(IESAttenuatedSource,
	//		"no light source specified, ignoring");
	//	return nullptr;
	//}

	//std::shared_ptr<TTexture<math::Spectrum>> attenuationTexture;
	//{
	//	const IesData iesData(m_iesFile);
	//
	//	const uint32 pixelsPerDegree = 2;
	//	const uint32 widthPx         = 360 * pixelsPerDegree;
	//	const uint32 heightPx        = 180 * pixelsPerDegree;

	//	TFrame<real, 1> attenuationFactors(widthPx, heightPx);
	//	for(uint32 y = 0; y < heightPx; y++)
	//	{
	//		for(uint32 x = 0; x < widthPx; x++)
	//		{
	//			const real u     = (static_cast<real>(x) + 0.5_r) / static_cast<real>(widthPx);
	//			const real v     = (static_cast<real>(y) + 0.5_r) / static_cast<real>(heightPx);
	//			const real phi   = u * math::constant::two_pi<real>;
	//			const real theta = (1.0_r - v) * math::constant::pi<real>;

	//			const real factor = iesData.sampleAttenuationFactor(theta, phi);
	//			attenuationFactors.setPixel(x, y, TFrame<real, 1>::Pixel(factor));
	//		}
	//	}

	//	auto attenuationFactorTexture = std::make_shared<TScalarPixelTexture2D<real>>(
	//		std::make_shared<TFrameBuffer2D<real, 1>>(attenuationFactors),
	//		0);
	//	
	//	// Convert sampled scalar to spectrum
	//	auto factorToSpectrumTexture = std::make_shared<TUnaryTextureOperator<
	//		math::TArithmeticArray<real, 1>, 
	//		math::Spectrum, 
	//		texfunc::TScalarToSpectrum<real>>>(attenuationFactorTexture);

	//	attenuationTexture = factorToSpectrumTexture;
	//}

	//auto sourceEmitter = m_source->genEmitter(ctx, std::move(data));
	//auto attenuatedEmitter = std::make_unique<OmniModulatedEmitter>(std::move(sourceEmitter));
	//attenuatedEmitter->setFilter(attenuationTexture);

	//return attenuatedEmitter;
	return nullptr;
}

std::shared_ptr<Geometry> IesAttenuatedSource::genGeometry(CookingContext& ctx) const
{
	if(!m_source)
	{
		return nullptr;
	}

	return m_source->genGeometry(ctx);
}

std::shared_ptr<Material> IesAttenuatedSource::genMaterial(CookingContext& ctx) const
{
	if(!m_source)
	{
		return nullptr;
	}

	return m_source->genMaterial(ctx);
}

}// end namespace ph
