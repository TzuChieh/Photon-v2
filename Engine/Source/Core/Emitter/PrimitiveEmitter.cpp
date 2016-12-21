#include "Core/Emitter/PrimitiveEmitter.h"
#include "Math/Vector3f.h"
#include "Entity/Geometry/Geometry.h"

namespace ph
{

PrimitiveEmitter::PrimitiveEmitter(const EmitterMetadata* const metadata, const Primitive* const primitive) :
	Emitter(metadata), 
	m_primitive(primitive)
{

}

PrimitiveEmitter::~PrimitiveEmitter()
{

}

void PrimitiveEmitter::samplePosition(const Vector3f& position, const Vector3f& direction, float32* const out_PDF, Vector3f* const out_emittedRadiance) const
{

}

}// end namespace ph