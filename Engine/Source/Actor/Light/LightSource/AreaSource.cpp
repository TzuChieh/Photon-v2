#include "Actor/Light/LightSource/AreaSource.h"
#include "Core/Primitive/PrimitiveStorage.h"
#include "Core/Emitter/EmitterStorage.h"
#include "Core/Emitter/EmitterMetadata.h"
#include "Actor/Model/Model.h"
#include "Core/Emitter/AreaEmitter.h"
#include "Image/ConstantTexture.h"

#include <iostream>

namespace ph
{

AreaSource::AreaSource(const Vector3f& emittedRadiance) : 
	m_emittedRadiance(std::make_shared<ConstantTexture>(emittedRadiance))
{

}

AreaSource::~AreaSource() = default;

void AreaSource::buildEmitters(PrimitiveStorage* const out_primitiveStorage,
                               EmitterStorage* const out_emitterStorage, 
                               const Model& lightModel) const
{
	if(!lightModel.getGeometry())
	{
		std::cerr << "warning: at AreaSource::buildEmitters(), AreaSource require a Geometry" << std::endl;
		return;
	}

	PrimitiveStorage primitiveStorage;
	//lightModel.cookData(&primitiveStorage);
	if(primitiveStorage.numPrimitives() == 0)
	{
		std::cerr << "warning: at AreaSource::buildEmitters(), no Primitive generated" << std::endl;
		return;
	}

	std::vector<const Primitive*> primitives;
	for(const auto& primitive : primitiveStorage)
	{
		primitives.push_back(primitive.get());
	}

	std::unique_ptr<EmitterMetadata> emitterMetadata = std::make_unique<EmitterMetadata>();
	emitterMetadata->m_worldToLocal = lightModel.getWorldToLocalTransform();
	emitterMetadata->m_localToWorld = lightModel.getLocalToWorldTransform();
	
	std::unique_ptr<AreaEmitter> emitter = std::make_unique<AreaEmitter>(emitterMetadata.get(), primitives);
	emitter->setEmittedRadiance(m_emittedRadiance);

	for(auto& primitiveMetadata : primitiveStorage.getMetadataBuffer())
	{
		//primitiveMetadata->m_surfaceBehavior->setEmitter(emitter.get());
	}

	out_primitiveStorage->add(std::move(primitiveStorage));
	out_emitterStorage->add(std::move(emitter));
	out_emitterStorage->add(std::move(emitterMetadata));
}

}// end namespace ph