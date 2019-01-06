#include "Actor/LightSource/IesAttenuatedSource.h"
#include "FileIO/Data/IesData.h"
#include "Core/Intersectable/UvwMapper/SphericalMapper.h"
#include "Core/Intersectable/PrimitiveMetadata.h"
#include "Core/Intersectable/PrimitiveChannel.h"
#include "Frame/TFrame.h"
#include "Core/Texture/TBilinearPixelTex2D.h"
#include "Core/Texture/Function/TConversionTexture.h"
#include "Math/constant.h"
#include "Core/Emitter/OmniModulatedEmitter.h"
#include "FileIO/SDL/InputPacket.h"

namespace ph
{

const Logger IesAttenuatedSource::logger(LogSender("IES Attenuated Source"));

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

IesAttenuatedSource::~IesAttenuatedSource() = default;

std::unique_ptr<Emitter> IesAttenuatedSource::genEmitter(
	CookingContext& context, EmitterBuildingMaterial&& data) const
{
	if(m_source == nullptr)
	{
		logger.log(ELogLevel::WARNING_MED, 
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
			const real phi   = u * 2.0_r * PH_PI_REAL;
			const real theta = (1.0_r - v) * PH_PI_REAL;

			const real factor = iesData.sampleAttenuationFactor(theta, phi);
			attenuationFactors.setPixel(x, y, TFrame<real, 1>::Pixel(factor));
		}
	}

	auto attenuationTexture = std::make_shared<TBilinearPixelTex2D<real, 1>>(attenuationFactors);
	auto sourceEmitter      = m_source->genEmitter(context, std::move(data));
	auto attenuatedEmitter  = std::make_unique<OmniModulatedEmitter>(std::move(sourceEmitter));
	auto convertedTexture   = std::make_shared<TConversionTexture<TTexPixel<real, 1>, SpectralStrength>>(attenuationTexture);

	attenuatedEmitter->setFilter(convertedTexture);

	return attenuatedEmitter;
}

std::shared_ptr<Geometry> IesAttenuatedSource::genGeometry(CookingContext& context) const
{
	if(m_source == nullptr)
	{
		return nullptr;
	}

	return m_source->genGeometry(context);
}

std::shared_ptr<Material> IesAttenuatedSource::genMaterial(CookingContext& context) const
{
	if(m_source == nullptr)
	{
		return nullptr;
	}

	return m_source->genMaterial(context);
}

// command interface

IesAttenuatedSource::IesAttenuatedSource(const InputPacket& packet) : 
	LightSource(packet),
	m_iesFile(), m_source(nullptr)
{
	m_iesFile = packet.getStringAsPath("ies-file", Path(), DataTreatment::REQUIRED());
	m_source  = packet.get<LightSource>("source", DataTreatment::REQUIRED());
}

SdlTypeInfo IesAttenuatedSource::ciTypeInfo()
{
	return SdlTypeInfo(ETypeCategory::REF_LIGHT_SOURCE, "ies-attenuated");
}

void IesAttenuatedSource::ciRegister(CommandRegister& cmdRegister)
{
	cmdRegister.setLoader(SdlLoader([](const InputPacket& packet)
	{
		return std::make_unique<IesAttenuatedSource>(packet);
	}));
}

}// end namespace ph