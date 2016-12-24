#include "Core/Primitive/Primitive.h"

#include <iostream>

namespace ph
{

Primitive::Primitive(const PrimitiveMetadata* const metadata) :
	m_metadata(metadata), m_reciExtendedArea(1.0f)
{
	
}

Primitive::~Primitive() = default;

}// end namespace ph