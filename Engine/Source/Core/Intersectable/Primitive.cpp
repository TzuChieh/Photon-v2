#include "Core/Intersectable/Primitive.h"
#include "Common/assertion.h"

#include <iostream>

namespace ph
{

Primitive::Primitive(const PrimitiveMetadata* const metadata) :
	m_metadata(metadata), m_reciExtendedArea(1.0f)
{
	PH_ASSERT(metadata != nullptr);
}

Primitive::~Primitive() = default;

}// end namespace ph