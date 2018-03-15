#include "Actor/LightSource/AreaSource.h"
#include "Actor/AModel.h"
#include "Core/Emitter/PrimitiveAreaEmitter.h"
#include "Core/Texture/TConstantTexture.h"
#include "Core/Texture/LdrRgbTexture2D.h"
#include "Core/Texture/TextureLoader.h"
#include "FileIO/InputPacket.h"
#include "Actor/LightSource/EmitterBuildingMaterial.h"
#include "Math/TVector3.h"
#include "FileIO/InputPrototype.h"
#include "FileIO/PictureLoader.h"
#include "Actor/Image/Image.h"
#include "Actor/Image/ConstantImage.h"
#include "Actor/Image/LdrPictureImage.h"
#include "Common/assertion.h"
#include "Core/Emitter/MultiAreaEmitter.h"
#include "Core/Intersectable/PrimitiveMetadata.h"
#include "Core/Intersectable/Primitive.h"

#include <iostream>

namespace ph
{

const Logger AreaSource::logger(LogSender("Area Source"));

AreaSource::AreaSource(const Vector3R& emittedRgbRadiance) :
	LightSource(), 
	m_emittedRadiance(nullptr)
{
	SpectralStrength radiance;
	radiance.setLinearSrgb(emittedRgbRadiance);// FIXME: check color space

	std::vector<real> vecRadianceSpectrum;
	for(std::size_t i = 0; i < SpectralStrength::NUM_VALUES; i++)
	{
		vecRadianceSpectrum.push_back(radiance[i]);
	}
	m_emittedRadiance = std::make_shared<ConstantImage>(vecRadianceSpectrum);
}

AreaSource::AreaSource(const Path& imagePath) : 
	LightSource(), 
	m_emittedRadiance(nullptr)
{
	auto image = std::make_shared<LdrPictureImage>(PictureLoader::loadLdr(imagePath));
	m_emittedRadiance = image;
}

AreaSource::AreaSource(const std::shared_ptr<Image>& emittedRadiance) :
	m_emittedRadiance(emittedRadiance)
{
	PH_ASSERT(m_emittedRadiance != nullptr);
}

AreaSource::~AreaSource() = default;

std::unique_ptr<Emitter> AreaSource::genEmitter(
	CookingContext& context, const EmitterBuildingMaterial& data) const
{
	if(data.primitives.empty())
	{
		logger.log(ELogLevel::WARNING_MED, 
		           "no primitive provided; requires at least a primitive to build emitter");
		return nullptr;
	}

	auto emittedRadiance = m_emittedRadiance->genTextureSpectral(context);

	std::vector<PrimitiveAreaEmitter> primitiveEmitters;
	for(const auto& primitive : data.primitives)
	{
		PrimitiveAreaEmitter emitter(primitive);
		emitter.setEmittedRadiance(emittedRadiance);
		primitiveEmitters.push_back(emitter);
	}

	PH_ASSERT(!primitiveEmitters.empty());

	if(primitiveEmitters.size() == 1)
	{
		return std::make_unique<PrimitiveAreaEmitter>(primitiveEmitters[0]);
	}
	else
	{
		auto multiEmitter = std::make_unique<MultiAreaEmitter>(std::move(primitiveEmitters));
		multiEmitter->setEmittedRadiance(emittedRadiance);
		return multiEmitter;
	}

	PH_ASSERT_UNREACHABLE_SECTION();
}

SdlTypeInfo AreaSource::ciTypeInfo()
{
	return SdlTypeInfo(ETypeCategory::REF_LIGHT_SOURCE, "area");
}

void AreaSource::ciRegister(CommandRegister& cmdRegister)
{
	SdlLoader loader;
	loader.setFunc<AreaSource>(ciLoad);
	cmdRegister.setLoader(loader);
}

std::unique_ptr<AreaSource> AreaSource::ciLoad(const InputPacket& packet)
{
	InputPrototype rgbInput;
	rgbInput.addVector3r("emitted-radiance");

	InputPrototype pictureFilenameInput;
	pictureFilenameInput.addString("emitted-radiance");

	if(packet.isPrototypeMatched(rgbInput))
	{
		const auto& emittedRadiance = packet.getVector3r(
			"emitted-radiance", Vector3R(0), DataTreatment::REQUIRED());
		return std::make_unique<AreaSource>(emittedRadiance);

	}
	else if(packet.isPrototypeMatched(pictureFilenameInput))
	{
		const auto& imagePath = packet.getStringAsPath(
			"emitted-radiance", Path(), DataTreatment::REQUIRED());
		return std::make_unique<AreaSource>(imagePath);
	}
	else
	{
		const auto& image = packet.get<Image>(
			"emitted-radiance", DataTreatment::REQUIRED());
		return std::make_unique<AreaSource>(image);
	}

	std::cerr << "warning: at AreaSource::ciLoad(), invalid input format" << std::endl;
	return std::make_unique<AreaSource>(Vector3R(1, 1, 1));
}

}// end namespace ph