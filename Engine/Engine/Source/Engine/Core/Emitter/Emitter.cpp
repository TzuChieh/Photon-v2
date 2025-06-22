#include "Engine/Core/Emitter/Emitter.h"
#include "Engine/Core/Transform/StaticAffineTransform.h"

namespace ph
{

Emitter::Emitter(const EmitterFeatureSet featureSet)
	: m_featureSet(featureSet)
{}

Emitter::~Emitter() = default;

}// end namespace ph
