#include "Core/Emitter/AreaEmitter.h"
#include "Math/Vector3f.h"
#include "Actor/Model/Geometry/Geometry.h"
#include "Core/Intersection.h"
#include "Image/ConstantTexture.h"

#include <iostream>

namespace ph
{

AreaEmitter::AreaEmitter(const EmitterMetadata* const metadata, const std::vector<const Primitive*>& primitives) :
	Emitter(metadata), 
	m_primitives(primitives), m_emittedRadiance(std::make_shared<ConstantTexture>(Vector3f(0, 0, 0)))
{
	if(primitives.empty())
	{
		std::cerr << "warning: at AreaEmitter::AreaEmitter(), no Primitive detected" << std::endl;
	}

	float64 extendedArea = 0.0;
	for(const auto& primitive : primitives)
	{
		extendedArea += 1.0f / primitive->getReciExtendedArea();
	}
	m_reciExtendedArea = static_cast<float32>(1.0 / extendedArea);
}

AreaEmitter::~AreaEmitter()
{

}

void AreaEmitter::evalEmittedRadiance(const Intersection& intersection, Vector3f* const out_emitterRadiance) const
{
	m_emittedRadiance->sample(intersection.getHitUVW(), out_emitterRadiance);
}

void AreaEmitter::samplePosition(const Vector3f& position, const Vector3f& direction, float32* const out_PDF, Vector3f* const out_emittedRadiance) const
{

}

void AreaEmitter::setEmittedRadiance(const std::shared_ptr<Texture> emittedRadiance)
{
	m_emittedRadiance = emittedRadiance;
}

}// end namespace ph