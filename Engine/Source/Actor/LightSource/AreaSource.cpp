#include "Actor/LightSource/AreaSource.h"
#include "Actor/AModel.h"
#include "Core/Emitter/PrimitiveAreaEmitter.h"
#include "Actor/Texture/ConstantTexture.h"
#include "Actor/Texture/PixelTexture.h"
#include "Actor/Texture/TextureLoader.h"
#include "Core/CoreActor.h"
#include "FileIO/InputPacket.h"
#include "Actor/LightSource/EmitterBuildingMaterial.h"

#include <iostream>

namespace ph
{

AreaSource::AreaSource(const Vector3f& emittedRadiance) : 
	LightSource(), 
	m_emittedRadiance(std::make_shared<ConstantTexture>(emittedRadiance))
{

}

AreaSource::AreaSource(const std::string& imageFilename) : 
	LightSource(), 
	m_emittedRadiance(std::make_shared<ConstantTexture>(Vector3f(0, 0, 0)))
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

AreaSource::AreaSource(const InputPacket& packet) : 
	LightSource(packet)
{
	//m_emittedRadiance = packet.getTexture("emitted-radiance", "AreaSource >> argument emitted-radiance not found");

	const Vector3f emittedRadiance = packet.getVector3r("emitted-radiance", Vector3f(0, 0, 0), 
	                                                    "AreaSource >> argument emitted-radiance not found");
	m_emittedRadiance = std::make_shared<ConstantTexture>(emittedRadiance);
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
	emitter->localToWorld = data.localToWorld;
	emitter->worldToLocal = data.worldToLocal;
	return std::move(emitter);
}

}// end namespace ph