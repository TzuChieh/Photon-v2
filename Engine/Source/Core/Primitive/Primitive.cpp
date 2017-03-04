#include "Core/Primitive/Primitive.h"

#include <iostream>

namespace ph
{

Primitive::Primitive(const PrimitiveMetadata* const metadata) :
	m_metadata(metadata), m_reciExtendedArea(1.0f)
{
	if(metadata == nullptr)
	{
		std::cerr << "warning: at Primitive::Primitive(), specified metadata is null" << std::endl;
	}
}

Primitive::~Primitive() = default;

}// end namespace ph