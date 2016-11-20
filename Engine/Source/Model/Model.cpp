#include "Model/Model.h"
#include "Math/Math.h"

namespace ph
{

Model::Model() : 
	m_geometry(nullptr), m_material(nullptr), 
	m_transformInfo(), m_modelToWorld(), m_worldToModel()
{

}

Model::Model(const std::shared_ptr<Geometry>& geometry, const std::shared_ptr<Material>& material) : 
	m_geometry(geometry), m_material(material), 
	m_transformInfo(), m_modelToWorld(), m_worldToModel()
{

}

Model::Model(const Model& other) : 
	m_geometry(other.m_geometry), m_material(other.m_material), 
	m_transformInfo(other.m_transformInfo), m_modelToWorld(other.m_modelToWorld), m_worldToModel(other.m_worldToModel)
{

}

void Model::translate(const Vector3f& translation)
{
	m_transformInfo.setPosition(m_transformInfo.getPosition().add(translation));

	updateTransforms();
}

void Model::translate(const float32 x, const float32 y, const float32 z)
{
	translate(Vector3f(x, y, z));
}

void Model::rotate(const Vector3f& normalizedAxis, const float32 degrees)
{
	Quaternion rotation(normalizedAxis, Math::toRadians(degrees));
	rotation.mulLocal(m_transformInfo.getRotation());
	rotation.normalizeLocal();
	m_transformInfo.setRotation(rotation);

	updateTransforms();
}

void Model::scale(const Vector3f& scaleFactor)
{
	m_transformInfo.setScale(m_transformInfo.getScale().mul(scaleFactor));

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

void Model::updateTransforms()
{
	m_modelToWorld = m_transformInfo.genTransform();
	m_worldToModel = m_transformInfo.genInverseTransform();
}

}// end namespace ph