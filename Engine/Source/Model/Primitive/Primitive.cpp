#include "Model/Primitive/Primitive.h"

namespace ph
{

Primitive::Primitive(const Model* const parentModel) : 
	m_parentModel(parentModel)
{

}

Primitive::~Primitive() = default;

}// end namespace ph