#include "Core/Emitter/Emitter.h"
#include "Math/Transform/StaticTransform.h"

namespace ph
{

Emitter::Emitter() : 
	m_localToWorld(&StaticTransform::IDENTITY()),
	m_worldToLocal(&StaticTransform::IDENTITY())
{
	
}

Emitter::~Emitter() = default;

void Emitter::setTransform(const Transform* localToWorld,
                           const Transform* worldToLocal)
{
	m_localToWorld = localToWorld;
	m_worldToLocal = worldToLocal;
}

}// end namespace ph