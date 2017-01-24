#include "Actor/AModel.h"
#include "Math/Math.h"
#include "Actor/TextureMapper/DefaultMapper.h"
#include "Core/Primitive/PrimitiveMetadata.h"
#include "Actor/Geometry/Geometry.h"
#include "Actor/Material/Material.h"
#include "Core/SurfaceBehavior/SurfaceBehavior.h"
#include "Core/CoreActor.h"
#include "FileIO/InputPacket.h"

#include <algorithm>
#include <iostream>

namespace ph
{

AModel::AModel() :
	m_geometry(nullptr), m_material(nullptr), 
	m_entityTransformInfo(), m_localToWorld(), m_worldToLocal()
{

}

AModel::AModel(const std::shared_ptr<Geometry>& geometry, const std::shared_ptr<Material>& material) :
	m_geometry(geometry), m_material(material), 
	m_entityTransformInfo(), m_localToWorld(), m_worldToLocal()
{

}

AModel::AModel(const AModel& other) :
	m_geometry(other.m_geometry), m_material(other.m_material), 
	m_entityTransformInfo(other.m_entityTransformInfo), m_localToWorld(other.m_localToWorld), m_worldToLocal(other.m_worldToLocal)
{
	
}

AModel::AModel(const InputPacket& packet) : 
	Actor(packet), 
	m_geometry(nullptr), m_material(nullptr), 
	m_entityTransformInfo(), m_localToWorld(), m_worldToLocal()
{
	m_geometry = packet.getGeometry("geometry", "AModel >> parameter geometry not found");
	m_material = packet.getMaterial("material", "AModel >> parameter material not found");
}

AModel::~AModel() = default;

void swap(AModel& first, AModel& second)
{
	// enable ADL
	using std::swap;

	// by swapping the members of two objects, the two objects are effectively swapped
	swap(first.m_geometry,            second.m_geometry);
	swap(first.m_material,            second.m_material);
	swap(first.m_entityTransformInfo, second.m_entityTransformInfo);
	swap(first.m_localToWorld,        second.m_localToWorld);
	swap(first.m_worldToLocal,        second.m_worldToLocal);
}

AModel& AModel::operator = (AModel rhs)
{
	swap(*this, rhs);

	return *this;
}

void AModel::genCoreActor(CoreActor* const out_coreActor) const
{
	if(m_geometry && m_material)
	{
		std::unique_ptr<PrimitiveMetadata> metadata = std::make_unique<PrimitiveMetadata>();
		metadata->worldToLocal = m_worldToLocal;
		metadata->localToWorld = m_localToWorld;

		std::vector<std::unique_ptr<Primitive>> primitives;
		m_geometry->discretize(&primitives, *metadata);
		m_material->populateSurfaceBehavior(&(metadata->surfaceBehavior));

		out_coreActor->primitives        = std::move(primitives);
		out_coreActor->primitiveMetadata = std::move(metadata);
	}
	else
	{
		std::cerr << "warning: at Model::genCoreActor(), incomplete data detected" << std::endl;
	}
}

void AModel::translate(const Vector3f& translation)
{
	m_entityTransformInfo.setPosition(m_entityTransformInfo.getPosition().add(translation));

	updateTransforms();
}

void AModel::translate(const float32 x, const float32 y, const float32 z)
{
	translate(Vector3f(x, y, z));
}

void AModel::rotate(const Vector3f& normalizedAxis, const float32 degrees)
{
	Quaternion rotation(normalizedAxis, Math::toRadians(degrees));
	rotation.mulLocal(m_entityTransformInfo.getRotation());
	rotation.normalizeLocal();
	m_entityTransformInfo.setRotation(rotation);

	updateTransforms();
}

void AModel::scale(const Vector3f& scaleFactor)
{
	m_entityTransformInfo.setScale(m_entityTransformInfo.getScale().mul(scaleFactor));

	updateTransforms();
}

void AModel::scale(const float32 x, const float32 y, const float32 z)
{
	scale(Vector3f(x, y, z));
}

void AModel::scale(const float32 scaleFactor)
{
	scale(Vector3f(scaleFactor));
}

void AModel::setGeometry(const std::shared_ptr<Geometry>& geometry)
{
	m_geometry = geometry;
}

void AModel::setMaterial(const std::shared_ptr<Material>& material)
{
	m_material = material;
}

void AModel::updateTransforms()
{
	m_localToWorld = m_entityTransformInfo.genTransform(Transform());
	m_worldToLocal = m_entityTransformInfo.genInverseTransform(Transform());
}

const Geometry* AModel::getGeometry() const
{
	return m_geometry.get();
}

const Material* AModel::getMaterial() const
{
	return m_material.get();
}

const Transform* AModel::getLocalToWorldTransform() const
{
	return &m_localToWorld;
}

const Transform* AModel::getWorldToLocalTransform() const
{
	return &m_worldToLocal;
}

}// end namespace ph