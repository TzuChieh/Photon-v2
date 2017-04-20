#include "Actor/LightSource/AreaSource.h"
#include "Actor/AModel.h"
#include "Core/Emitter/PrimitiveAreaEmitter.h"
#include "Actor/Texture/ConstantTexture.h"
#include "Actor/Texture/PixelTexture.h"
#include "Actor/Texture/TextureLoader.h"
#include "Core/CookedActor.h"
#include "FileIO/InputPacket.h"
#include "Actor/LightSource/EmitterBuildingMaterial.h"
#include "Math/TVector3.h"

#include <iostream>

namespace ph
{

AreaSource::AreaSource(const Vector3R& emittedRadiance) :
	LightSource(), 
	m_emittedRadiance(std::make_shared<ConstantTexture>(emittedRadiance))
{

}

AreaSource::AreaSource(const std::string& imageFilename) : 
	LightSource(), 
	m_emittedRadiance(std::make_shared<ConstantTexture>(Vector3R(0, 0, 0)))
{
	std::shared_ptr<PixelTexture> image = std::make_shared<PixelTexture>();
	TextureLoader loader;
	if(loader.load(imageFilename, image.get()))
	{
		m_emittedRadiance = image;
	}
	else
	{
		std::cerr << "warning: at AreaSource::AreaSource(), image loading failed" << std::endl;
	}
}

AreaSource::~AreaSource() = default;

std::unique_ptr<Emitter> AreaSource::buildEmitter(const EmitterBuildingMaterial& data) const
{
	if(data.primitives.empty())
	{
		std::cerr << "warning: at AreaSource::buildEmitter(), requires at least a Primitive to build" << std::endl;
		return nullptr;
	}
	
	std::unique_ptr<PrimitiveAreaEmitter> emitter = std::make_unique<PrimitiveAreaEmitter>(data.primitives);
	emitter->setEmittedRadiance(m_emittedRadiance);
	return std::move(emitter);
}

AreaSource::AreaSource(const InputPacket& packet) :
	LightSource(packet)
{
	const Vector3R emittedRadiance = packet.getVector3r("emitted-radiance", Vector3R(0), DataTreatment::REQUIRED());
	m_emittedRadiance = std::make_shared<ConstantTexture>(emittedRadiance);
}

SdlTypeInfo AreaSource::ciTypeInfo()
{
	return SdlTypeInfo(ETypeCategory::REF_LIGHT_SOURCE, "area");
}

ExitStatus AreaSource::ciExecute(const std::shared_ptr<AreaSource>& targetResource, const std::string& functionName, const InputPacket& packet)
{
	return ExitStatus::UNSUPPORTED();
}

}// end namespace ph