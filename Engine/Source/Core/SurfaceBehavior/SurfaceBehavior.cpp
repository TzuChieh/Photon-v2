#include "Core/SurfaceBehavior/SurfaceBehavior.h"

#include <iostream>

namespace ph
{

SurfaceBehavior::SurfaceBehavior() : 
	m_surfaceOptics(nullptr), m_emitter(nullptr)
{

}

void SurfaceBehavior::setSurfaceOptics(std::unique_ptr<SurfaceOptics> optics)
{
	m_surfaceOptics = std::move(optics);
}

void SurfaceBehavior::setEmitter(const Emitter* emitter)
{
	m_emitter = emitter;
}

}// end namespace ph