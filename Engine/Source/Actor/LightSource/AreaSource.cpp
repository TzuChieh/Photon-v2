#include "Actor/LightSource/AreaSource.h"
#include "Actor/AModel.h"
#include "Core/Emitter/PrimitiveAreaEmitter.h"
#include "Actor/Texture/ConstantTexture.h"
#include "Actor/Texture/PixelTexture.h"
#include "Actor/Texture/TextureLoader.h"
#include "Core/CoreActor.h"
#include "FileIO/InputPacket.h"

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
	m_emittedRadiance = packet.getTexture("emitted-radiance", "AreaSource >> argument emitted-radiance not found");
}

AreaSource::~AreaSource() = default;

void AreaSource::buildEmitter(CoreActor& coreActor) const
{
	if(!checkCoreActor(coreActor))
	{
		return;
	}

	std::vector<const Primitive*> primitives;
	for(const auto& primitive : coreActor.primitives)
	{
		primitives.push_back(primitive.get());
	}
	
	std::unique_ptr<PrimitiveAreaEmitter> emitter = std::make_unique<PrimitiveAreaEmitter>(primitives);
	emitter->setEmittedRadiance(m_emittedRadiance);
	emitter->worldToLocal = coreActor.primitiveMetadata->worldToLocal;
	emitter->localToWorld = coreActor.primitiveMetadata->localToWorld;

	coreActor.primitiveMetadata->surfaceBehavior.setEmitter(emitter.get());
	coreActor.emitter = std::move(emitter);
}

bool AreaSource::checkCoreActor(const CoreActor& coreActor)
{
	// check errors

	if(coreActor.primitives.empty())
	{
		std::cerr << "warning: at AreaSource::checkCoreActor(), AreaSource require at least a Primitive" << std::endl;
		return false;
	}

	if(coreActor.primitiveMetadata == nullptr)
	{
		std::cerr << "warning: at AreaSource::checkCoreActor(), AreaSource has no PrimitiveMetadata for reference" << std::endl;
		return false;
	}

	// emit warnings

	if(coreActor.emitter != nullptr)
	{
		std::cerr << "warning: at AreaSource::checkCoreActor(), going to override an existing emitter found in CoreActor" << std::endl;
	}

	return true;
}

}// end namespace ph