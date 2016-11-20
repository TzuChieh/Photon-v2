#include "Model/Primitive/Primitive.h"
#include "Model/Model.h"

namespace ph
{

Primitive::Primitive(const Model* const parentModel) : 
	m_parentModel(parentModel)
{

}

Primitive::~Primitive() = default;

const Transform* Primitive::getModelToWorldTransform() const
{
	return m_parentModel->getModelToWorldTransform();
}

const Transform* Primitive::getWorldToModelTransform() const
{
	return m_parentModel->getWorldToModelTransform();
}

}// end namespace ph