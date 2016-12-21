#include "Core/Emitter/EmitterMetadata.h"

namespace ph
{

EmitterMetadata::EmitterMetadata() :
	m_material(nullptr), m_localToWorld(nullptr), m_worldToLocal(nullptr), m_textureMapper(nullptr)
{

}

}// end namespace ph