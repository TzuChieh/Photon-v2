#include "Entity/Entity.h"
#include "Math/Math.h"
#include "Entity/TextureMapper/DefaultMapper.h"

#include <algorithm>
#include <iostream>

namespace ph
{

Entity::Entity() :
	m_geometry(nullptr), m_material(nullptr), m_textureMapper(std::make_shared<DefaultMapper>()), m_emitter(nullptr), 
	m_entityTransformInfo(), m_localToWorld(), m_worldToLocal(), 
	m_parentEntity(nullptr)
{

}

Entity::Entity(const std::shared_ptr<Geometry>& geometry, const std::shared_ptr<Material>& material) :
	m_geometry(geometry), m_material(material), m_textureMapper(std::make_shared<DefaultMapper>()), m_emitter(nullptr), 
	m_entityTransformInfo(), m_localToWorld(), m_worldToLocal(),
	m_parentEntity(nullptr)
{

}

Entity::Entity(const Entity& other) :
	m_geometry(other.m_geometry), m_material(other.m_material), m_textureMapper(other.m_textureMapper), m_emitter(other.m_emitter), 
	m_entityTransformInfo(other.m_entityTransformInfo), m_localToWorld(other.m_localToWorld), m_worldToLocal(other.m_worldToLocal),
	m_childrenEntities(other.m_childrenEntities), m_parentEntity(nullptr)
{
	for(Entity& children : m_childrenEntities)
	{
		children.setParent(this);
	}
}

Entity::~Entity() = default;

void swap(Entity& first, Entity& second)
{
	// enable ADL
	using std::swap;

	// by swapping the members of two objects, the two objects are effectively swapped
	swap(first.m_geometry,            second.m_geometry);
	swap(first.m_material,            second.m_material);
	swap(first.m_textureMapper,       second.m_textureMapper);
	swap(first.m_emitter,             second.m_emitter);
	swap(first.m_entityTransformInfo, second.m_entityTransformInfo);
	swap(first.m_localToWorld,        second.m_localToWorld);
	swap(first.m_worldToLocal,        second.m_worldToLocal);
	swap(first.m_childrenEntities,    second.m_childrenEntities);
	swap(first.m_parentEntity,        second.m_parentEntity);
}

Entity& Entity::operator = (Entity rhs)
{
	const Entity* parent = m_parentEntity;

	swap(*this, rhs);

	// retain original parent and concatenate assigned children
	setParent(parent);
	for(Entity& children : m_childrenEntities)
	{
		children.setParent(this);
	}

	return *this;
}

void Entity::translate(const Vector3f& translation)
{
	m_entityTransformInfo.setPosition(m_entityTransformInfo.getPosition().add(translation));

	updateAllTransforms();
}

void Entity::translate(const float32 x, const float32 y, const float32 z)
{
	translate(Vector3f(x, y, z));
}

void Entity::rotate(const Vector3f& normalizedAxis, const float32 degrees)
{
	Quaternion rotation(normalizedAxis, Math::toRadians(degrees));
	rotation.mulLocal(m_entityTransformInfo.getRotation());
	rotation.normalizeLocal();
	m_entityTransformInfo.setRotation(rotation);

	updateAllTransforms();
}

void Entity::scale(const Vector3f& scaleFactor)
{
	m_entityTransformInfo.setScale(m_entityTransformInfo.getScale().mul(scaleFactor));

	updateAllTransforms();
}

void Entity::scale(const float32 x, const float32 y, const float32 z)
{
	scale(Vector3f(x, y, z));
}

void Entity::scale(const float32 scaleFactor)
{
	scale(Vector3f(scaleFactor));
}

void Entity::setGeometry(const std::shared_ptr<Geometry>& geometry)
{
	m_geometry = geometry;
}

void Entity::setMaterial(const std::shared_ptr<Material>& material)
{
	m_material = material;
}

void Entity::setTextureMapper(const std::shared_ptr<TextureMapper>& textureMapper)
{
	m_textureMapper = textureMapper;
}

void Entity::setEmitter(const std::shared_ptr<Emitter>& emitter)
{
	m_emitter = emitter;
}

bool Entity::hasChildren() const
{
	return !(m_childrenEntities.empty());
}

std::vector<Entity>& Entity::getChildren()
{
	return m_childrenEntities;
}

const std::vector<Entity>& Entity::getChildren() const
{
	return m_childrenEntities;
}

void Entity::updateAllTransforms()
{
	updateTransforms();

	for(Entity& children : m_childrenEntities)
	{
		children.updateAllTransforms();
	}
}

void Entity::updateTransforms()
{
	const Transform& parentTransform    = m_parentEntity ? *(m_parentEntity->getLocalToWorldTransform()) : Transform::EMPTY_TRANSFORM;
	const Transform& parentInvTransform = m_parentEntity ? *(m_parentEntity->getWorldToLocalTransform()) : Transform::EMPTY_TRANSFORM;

	m_localToWorld = m_entityTransformInfo.genTransform(parentTransform);
	m_worldToLocal = m_entityTransformInfo.genInverseTransform(parentInvTransform);
}

void Entity::setParent(const Entity* const parent)
{
	m_parentEntity = parent;
	updateAllTransforms();
}

void Entity::addChild(const Entity& child)
{
	m_childrenEntities.push_back(child);
	m_childrenEntities.back().setParent(this);
}

const Geometry* Entity::getGeometry() const
{
	return m_geometry.get();
}

const Material* Entity::getMaterial() const
{
	return m_material.get();
}

const TextureMapper* Entity::getTextureMapper() const
{
	return m_textureMapper.get();
}

const Emitter* Entity::getEmitter() const
{
	return m_emitter.get();
}

const Transform* Entity::getLocalToWorldTransform() const
{
	return &m_localToWorld;
}

const Transform* Entity::getWorldToLocalTransform() const
{
	return &m_worldToLocal;
}

}// end namespace ph