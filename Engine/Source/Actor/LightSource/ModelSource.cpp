#include "Actor/LightSource/ModelSource.h"
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

const Logger ModelSource::logger(LogSender("Model Source"));

ModelSource::ModelSource(const Vector3R& emittedRgbRadiance) :
	LightSource(), 
	m_emittedRadiance(nullptr)
{
	m_emittedRadiance = std::make_shared<ConstantImage>(emittedRgbRadiance, 
	                                                    ConstantImage::EType::EMR_LINEAR_SRGB);
}

ModelSource::ModelSource(const Path& imagePath) :
	LightSource(), 
	m_emittedRadiance(nullptr)
{
	auto image = std::make_shared<LdrPictureImage>(PictureLoader::loadLdr(imagePath));
	m_emittedRadiance = image;
}

ModelSource::ModelSource(const std::shared_ptr<Image>& emittedRadiance) :
	m_emittedRadiance(emittedRadiance)
{
	PH_ASSERT(m_emittedRadiance != nullptr);
}

ModelSource::~ModelSource() = default;

std::unique_ptr<Emitter> ModelSource::genEmitter(
	CookingContext& context, EmitterBuildingMaterial&& data) const
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

SdlTypeInfo ModelSource::ciTypeInfo()
{
	return SdlTypeInfo(ETypeCategory::REF_LIGHT_SOURCE, "model");
}

void ModelSource::ciRegister(CommandRegister& cmdRegister)
{
	SdlLoader loader;
	loader.setFunc<ModelSource>(ciLoad);
	cmdRegister.setLoader(loader);
}

std::unique_ptr<ModelSource> ModelSource::ciLoad(const InputPacket& packet)
{
	InputPrototype rgbInput;
	rgbInput.addVector3r("emitted-radiance");

	InputPrototype pictureFilenameInput;
	pictureFilenameInput.addString("emitted-radiance");

	if(packet.isPrototypeMatched(rgbInput))
	{
		const auto& emittedRadiance = packet.getVector3r(
			"emitted-radiance", Vector3R(0), DataTreatment::REQUIRED());
		return std::make_unique<ModelSource>(emittedRadiance);

	}
	else if(packet.isPrototypeMatched(pictureFilenameInput))
	{
		const auto& imagePath = packet.getStringAsPath(
			"emitted-radiance", Path(), DataTreatment::REQUIRED());
		return std::make_unique<ModelSource>(imagePath);
	}
	else
	{
		const auto& image = packet.get<Image>(
			"emitted-radiance", DataTreatment::REQUIRED());
		return std::make_unique<ModelSource>(image);
	}

	std::cerr << "warning: at ModelSource::ciLoad(), invalid input format" << std::endl;
	return std::make_unique<ModelSource>(Vector3R(1, 1, 1));
}

}// end namespace ph