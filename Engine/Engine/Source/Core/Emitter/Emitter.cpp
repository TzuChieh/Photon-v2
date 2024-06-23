#include "Core/Emitter/Emitter.h"
#include "Math/Transform/StaticAffineTransform.h"

namespace ph
{

Emitter::Emitter(const EmitterFeatureSet featureSet)
	: m_featureSet(featureSet)
{}

Emitter::~Emitter() = default;

}// end namespace ph
