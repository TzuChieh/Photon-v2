#include "Core/SurfaceBehavior/SurfaceBehavior.h"
#include "Common/assertion.h"

#include <utility>

namespace ph
{

SurfaceBehavior::SurfaceBehavior() : 
	m_optics(nullptr), m_emitter(nullptr)
{}

void SurfaceBehavior::setOptics(std::shared_ptr<SurfaceOptics> optics)
{
	PH_ASSERT(optics);

	m_optics = std::move(optics);
}

void SurfaceBehavior::setEmitter(const Emitter* const emitter)
{
	PH_ASSERT(emitter);

	m_emitter = emitter;
}

std::shared_ptr<SurfaceOptics> SurfaceBehavior::getOpticsResource() const
{
	return m_optics;
}

}// end namespace ph
