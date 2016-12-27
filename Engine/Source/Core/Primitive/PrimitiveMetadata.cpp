#include "Core/Primitive/PrimitiveMetadata.h"

#include <iostream>

namespace ph
{

PrimitiveMetadata::PrimitiveMetadata() :
	m_surfaceBehavior(nullptr), m_localToWorld(nullptr), m_worldToLocal(nullptr)
{

}

}// end namespace ph