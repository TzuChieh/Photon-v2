#include "Engine/Actor/Material/AbradedOpaque.h"
#include "Engine/Core/SurfaceBehavior/SurfaceBehavior.h"
#include "Engine/Core/SurfaceBehavior/SurfaceOptics/OpaqueMicrofacet.h"

#include <memory>

namespace ph
{

AbradedOpaque::AbradedOpaque() : 

	SurfaceMaterial(),

	m_interfaceInfo(),
	m_microsurfaceInfo()
{}

void AbradedOpaque::genSurface(const CookingContext& ctx, SurfaceBehavior& behavior) const
{
	behavior.setOptics(
		std::make_unique<OpaqueMicrofacet>(
			m_interfaceInfo.genFresnelEffect(),
			m_microsurfaceInfo.genMicrofacet()));

	// TODO: generate ideal reflector if roughness == 0
}

}// end namespace ph
