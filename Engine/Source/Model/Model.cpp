#include "Model/Model.h"
#include "Math/Math.h"
#include "Model/TextureMapper/DefaultMapper.h"

#include <algorithm>
#include <iostream>

namespace ph
{

Model::Model() : 
	m_geometry(nullptr), m_material(nullptr), m_textureMapper(nullptr), 
	m_modelTransformInfo(), m_modelToWorld(), m_worldToModel(), 
	m_parentModel(nullptr)
{

}

Model::Model(const std::shared_ptr<Geometry>& geometry, const std::shared_ptr<Material>& material) : 
	m_geometry(geometry), m_material(material), m_textureMapper(std::make_shared<DefaultMapper>()), 
	m_modelTransformInfo(), m_modelToWorld(), m_worldToModel(),
	m_parentModel(nullptr)
{

}

Model::Model(const Model& other) : 
	m_geometry(other.m_geometry), m_material(other.m_material), m_textureMapper(other.m_textureMapper), 
	m_modelTransformInfo(other.m_modelTransformInfo), m_modelToWorld(other.m_modelToWorld), m_worldToModel(other.m_worldToModel),
	m_childrenModels(other.m_childrenModels), m_parentModel(nullptr)
{
	for(Model& children : m_childrenModels)
	{
		children.setParent(this);
	}
}

Model::~Model() = default;

void swap(Model& first, Model& second)
{
	// enable ADL
	using std::swap;

	// by swapping the members of two objects, the two objects are effectively swapped
	swap(first.m_geometry,           second.m_geometry);
	swap(first.m_material,           second.m_material);
	swap(first.m_textureMapper,      second.m_textureMapper);
	swap(first.m_modelTransformInfo, second.m_modelTransformInfo);
	swap(first.m_modelToWorld,       second.m_modelToWorld);
	swap(first.m_worldToModel,       second.m_worldToModel);
	swap(first.m_childrenModels,     second.m_childrenModels);
	swap(first.m_parentModel,        second.m_parentModel);
}

Model& Model::operator = (Model rhs)
{
	const Model* parent = m_parentModel;

	swap(*this, rhs);

	// retain original parent and concatenate assigned children
	setParent(parent);
	for(Model& children : m_childrenModels)
	{
		children.setParent(this);
	}

	return *this;
}

void Model::translate(const Vector3f& translation)
{
	m_modelTransformInfo.setPosition(m_modelTransformInfo.getPosition().add(translation));

	updateAllTransforms();
}

void Model::translate(const float32 x, const float32 y, const float32 z)
{
	translate(Vector3f(x, y, z));
}

void Model::rotate(const Vector3f& normalizedAxis, const float32 degrees)
{
	Quaternion rotation(normalizedAxis, Math::toRadians(degrees));
	rotation.mulLocal(m_modelTransformInfo.getRotation());
	rotation.normalizeLocal();
	m_modelTransformInfo.setRotation(rotation);

	updateAllTransforms();
}

void Model::scale(const Vector3f& scaleFactor)
{
	m_modelTransformInfo.setScale(m_modelTransformInfo.getScale().mul(scaleFactor));

	updateAllTransforms();
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

void Model::setTextureMapper(const std::shared_ptr<TextureMapper>& textureMapper)
{
	m_textureMapper = textureMapper;
}

bool Model::hasChildren() const
{
	return !(m_childrenModels.empty());
}

std::vector<Model>& Model::getChildren()
{
	return m_childrenModels;
}

const std::vector<Model>& Model::getChildren() const
{
	return m_childrenModels;
}

void Model::updateAllTransforms()
{
	updateTransforms();

	for(Model& children : m_childrenModels)
	{
		children.updateAllTransforms();
	}
}

void Model::updateTransforms()
{
	const Transform& parentTransform    = m_parentModel ? *(m_parentModel->getModelToWorldTransform()) : Transform::EMPTY_TRANSFORM;
	const Transform& parentInvTransform = m_parentModel ? *(m_parentModel->getWorldToModelTransform()) : Transform::EMPTY_TRANSFORM;

	m_modelToWorld = m_modelTransformInfo.genTransform(parentTransform);
	m_worldToModel = m_modelTransformInfo.genInverseTransform(parentInvTransform);
}

void Model::setParent(const Model* const parent)
{
	if(!parent)
	{
		std::cerr << "warning: at Model::setParent(), parent is null" << std::endl;
	}

	m_parentModel = parent;
	updateAllTransforms();
}

void Model::addChild(const Model& child)
{
	m_childrenModels.push_back(child);
	m_childrenModels.back().setParent(this);
}

}// end namespace ph