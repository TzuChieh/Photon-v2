#include "Actor/Model/Model.h"
#include "Math/Math.h"
#include "Actor/Model/TextureMapper/DefaultMapper.h"
#include "Core/Primitive/PrimitiveMetadata.h"
#include "Actor/Model/Geometry/Geometry.h"
#include "Actor/Model/Material/Material.h"
#include "Core/SurfaceBehavior/SurfaceBehavior.h"
#include "Core/CookedModelStorage.h"

#include <algorithm>
#include <iostream>

namespace ph
{

Model::Model() :
	m_geometry(nullptr), m_material(nullptr), 
	m_entityTransformInfo(), m_localToWorld(), m_worldToLocal()
{

}

Model::Model(const std::shared_ptr<Geometry>& geometry, const std::shared_ptr<Material>& material) :
	m_geometry(geometry), m_material(material), 
	m_entityTransformInfo(), m_localToWorld(), m_worldToLocal()
{

}

Model::Model(const Model& other) :
	m_geometry(other.m_geometry), m_material(other.m_material), 
	m_entityTransformInfo(other.m_entityTransformInfo), m_localToWorld(other.m_localToWorld), m_worldToLocal(other.m_worldToLocal)
{
	
}

Model::~Model() = default;

void swap(Model& first, Model& second)
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

Model& Model::operator = (Model rhs)
{
	swap(*this, rhs);

	return *this;
}

void Model::cookData(CookedModelStorage* const out_cookedModelStorage) const
{
	if(m_geometry && m_material)
	{
		std::unique_ptr<PrimitiveMetadata> metadata = std::make_unique<PrimitiveMetadata>();
		metadata->worldToLocal  = m_worldToLocal;
		metadata->localToWorld  = m_localToWorld;

		std::vector<std::unique_ptr<Primitive>> primitives;
		m_geometry->discretize(&primitives, *metadata);
		m_material->populateSurfaceBehavior(&(metadata->surfaceBehavior));

		out_cookedModelStorage->add(std::move(primitives));
		out_cookedModelStorage->add(std::move(metadata));
	}
}

void Model::translate(const Vector3f& translation)
{
	m_entityTransformInfo.setPosition(m_entityTransformInfo.getPosition().add(translation));

	updateTransforms();
}

void Model::translate(const float32 x, const float32 y, const float32 z)
{
	translate(Vector3f(x, y, z));
}

void Model::rotate(const Vector3f& normalizedAxis, const float32 degrees)
{
	Quaternion rotation(normalizedAxis, Math::toRadians(degrees));
	rotation.mulLocal(m_entityTransformInfo.getRotation());
	rotation.normalizeLocal();
	m_entityTransformInfo.setRotation(rotation);

	updateTransforms();
}

void Model::scale(const Vector3f& scaleFactor)
{
	m_entityTransformInfo.setScale(m_entityTransformInfo.getScale().mul(scaleFactor));

	updateTransforms();
}

void Model::scale(const float32 x, const float32 y, const float32 z)
{
	scale(Vector3f(x, y, z));
}

void Model::scale(const float32 scaleFactor)
{
	scale(Vector3f(scaleFactor));
}

void Model::setGeometry(const std::shared_ptr<Geometry>& geometry)
{
	m_geometry = geometry;
}

void Model::setMaterial(const std::shared_ptr<Material>& material)
{
	m_material = material;
}

void Model::updateTransforms()
{
	m_localToWorld = m_entityTransformInfo.genTransform(Transform());
	m_worldToLocal = m_entityTransformInfo.genInverseTransform(Transform());
}

const Geometry* Model::getGeometry() const
{
	return m_geometry.get();
}

const Material* Model::getMaterial() const
{
	return m_material.get();
}

const Transform* Model::getLocalToWorldTransform() const
{
	return &m_localToWorld;
}

const Transform* Model::getWorldToLocalTransform() const
{
	return &m_worldToLocal;
}

}// end namespace ph