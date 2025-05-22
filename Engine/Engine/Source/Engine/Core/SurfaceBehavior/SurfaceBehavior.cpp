#include "Engine/Core/SurfaceBehavior/SurfaceBehavior.h"

#include <Common/assertion.h>

namespace ph
{

SurfaceBehavior::SurfaceBehavior() : 
	m_optics(nullptr), m_emitter(nullptr)
{}

void SurfaceBehavior::setOptics(const SurfaceOptics* optics)
{
	PH_ASSERT(optics);

	m_optics = optics;
}

void SurfaceBehavior::setEmitter(const Emitter* const emitter)
{
	PH_ASSERT(emitter);

	m_emitter = emitter;
}

}// end namespace ph
