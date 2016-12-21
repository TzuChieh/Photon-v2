#include "Entity/Primitive/PrimitiveMetadata.h"

#include <iostream>

namespace ph
{

PrimitiveMetadata::PrimitiveMetadata() :
	m_material(nullptr), m_localToWorld(nullptr), m_worldToLocal(nullptr), m_textureMapper(nullptr)
{

}

}// end namespace ph