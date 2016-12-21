#include "Core/Emitter/Emitter.h"

namespace ph
{

Emitter::Emitter(const EmitterMetadata* const metadata) : 
	m_metadata(metadata)
{

}

Emitter::~Emitter() = default;

}// end namespace ph