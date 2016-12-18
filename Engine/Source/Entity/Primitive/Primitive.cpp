#include "Entity/Primitive/Primitive.h"

namespace ph
{

Primitive::Primitive(const Entity* const parentEntity) : 
	m_parentEntity(parentEntity)
{

}

Primitive::~Primitive() = default;

}// end namespace ph