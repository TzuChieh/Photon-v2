#include "Actor/Material/AbradedTranslucent.h"
#include "Core/SurfaceBehavior/SurfaceBehavior.h"
#include "Core/SurfaceBehavior/SurfaceOptics/TranslucentMicrofacet.h"

#include <memory>

namespace ph
{

AbradedTranslucent::AbradedTranslucent() :
	SurfaceMaterial(),

	m_interfaceInfo   (),
	m_microsurfaceInfo()
{}

void AbradedTranslucent::genSurface(const CookingContext& ctx, SurfaceBehavior& behavior) const
{
	behavior.setOptics(
		std::make_unique<TranslucentMicrofacet>(
			m_interfaceInfo.genFresnelEffect(),
			m_microsurfaceInfo.genMicrofacet()));

	// TODO: generate ideal dielectric if roughness == 0
}

}// end namespace ph
