#include "Engine/Core/SurfaceBehavior/SurfaceBehavior.h"

namespace ph
{

SurfaceBehavior::SurfaceBehavior() : 
	m_optics(nullptr), m_emitter(nullptr)
{}

void SurfaceBehavior::setOptics(const SurfaceOptics* optics)
{
	m_optics = optics;
}

void SurfaceBehavior::setEmitter(const Emitter* const emitter)
{
	m_emitter = emitter;
}

}// end namespace ph
