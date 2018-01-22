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

#include <iostream>

namespace ph
{

AreaSource::AreaSource(const Vector3R& emittedRgbRadiance) :
	LightSource(), 
	m_emittedRadiance(nullptr)
{
	SpectralStrength radiance;
	radiance.setRgb(emittedRgbRadiance);

	std::vector<real> vecRadianceSpectrum;
	for(std::size_t i = 0; i < SpectralStrength::numElements(); i++)
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
		std::cerr << "warning: at AreaSource::buildEmitter(), requires at least a Primitive to build" << std::endl;
		return nullptr;
	}
	
	std::unique_ptr<PrimitiveAreaEmitter> emitter = std::make_unique<PrimitiveAreaEmitter>(data.primitives);
	std::shared_ptr<TTexture<SpectralStrength>> radianceTexture;
	m_emittedRadiance->genTextureSpectral(context, &radianceTexture);
	emitter->setEmittedRadiance(radianceTexture);
	return std::move(emitter);
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
	/*else
	{
		const auto& image = packet.get<Image>(
			"emitted-radiance", DataTreatment::REQUIRED());
		auto source = std::make_unique<AreaSource>(image);
	}*/

	std::cerr << "warning: at AreaSource::ciLoad(), invalid input format" << std::endl;
	return std::make_unique<AreaSource>(Vector3R(1, 1, 1));
}

}// end namespace ph