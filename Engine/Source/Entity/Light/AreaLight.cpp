#include "Entity/Light/AreaLight.h"
#include "Core/Emitter/EmitterMetadata.h"
#include "Core/Emitter/PrimitiveEmitter.h"
#include "Core/Primitive/PrimitiveStorage.h"
#include "Core/Emitter/EmitterStorage.h"
#include "Entity/Entity.h"
#include "Entity/Geometry/Geometry.h"

#include <iostream>
#include <vector>

namespace ph
{

AreaLight::~AreaLight() = default;

void AreaLight::buildEmitters(EmitterStorage* const out_data, const Entity& parentEntity) const
{
	if(!checkEntityCompleteness(parentEntity))
	{
		return;
	}

	PrimitiveStorage primitives;
	parentEntity.getGeometry()->discretize(&primitives, parentEntity);

	auto emitterMetadata = std::make_unique<EmitterMetadata>();
	emitterMetadata->m_localToWorld  = parentEntity.getLocalToWorldTransform();
	emitterMetadata->m_worldToLocal  = parentEntity.getWorldToLocalTransform();
	emitterMetadata->m_material      = parentEntity.getMaterial();
	emitterMetadata->m_textureMapper = parentEntity.getTextureMapper();

	for(const auto& primitive : primitives)
	{
		out_data->add(std::make_unique<PrimitiveEmitter>(emitterMetadata.get(), primitive.get()));
	}
	
	out_data->add(std::move(emitterMetadata));
	out_data->add(std::move(primitives));
}

bool AreaLight::checkEntityCompleteness(const Entity& entity)
{
	if(!entity.getGeometry() || !entity.getMaterial() || !entity.getTextureMapper() || !entity.getLight() || 
	   !entity.getLocalToWorldTransform() || !entity.getWorldToLocalTransform())
	{
		std::cerr << "warning: at AreaLight::checkEntityCompleteness(), incomplete entity detected" << std::endl;
		return false;
	}

	return true;
}

}// end namespace ph