#include "Actor/Material/AbradedOpaque.h"
#include "Core/SurfaceBehavior/SurfaceBehavior.h"
#include "Core/SurfaceBehavior/SurfaceOptics/OpaqueMicrofacet.h"

#include <memory>

namespace ph
{

AbradedOpaque::AbradedOpaque() : 

	SurfaceMaterial(),

	m_interfaceInfo(),
	m_microsurfaceInfo()
{}

void AbradedOpaque::genSurface(ActorCookingContext& ctx, SurfaceBehavior& behavior) const
{
	behavior.setOptics(
		std::make_unique<OpaqueMicrofacet>(
			m_interfaceInfo.genFresnelEffect(),
			m_microsurfaceInfo.genMicrofacet()));
}

}// end namespace ph
