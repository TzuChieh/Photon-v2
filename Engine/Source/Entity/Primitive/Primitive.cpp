#include "Entity/Primitive/Primitive.h"

#include <iostream>

namespace ph
{

Primitive::Primitive(const PrimitiveMetadata* const metadata) :
	m_metadata(metadata)
{
	
}

Primitive::~Primitive() = default;

}// end namespace ph