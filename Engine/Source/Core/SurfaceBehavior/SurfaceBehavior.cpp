#include "Core/SurfaceBehavior/SurfaceBehavior.h"
#include "Common/assertion.h"

namespace ph
{

SurfaceBehavior::SurfaceBehavior() : 
	m_optics(nullptr), m_emitter(nullptr)
{}

void SurfaceBehavior::setOptics(const std::shared_ptr<SurfaceOptics>& optics)
{
	PH_ASSERT(optics != nullptr);

	m_optics = optics;
}

void SurfaceBehavior::setEmitter(const Emitter* const emitter)
{
	PH_ASSERT(emitter != nullptr);

	m_emitter = emitter;
}

}// end namespace ph