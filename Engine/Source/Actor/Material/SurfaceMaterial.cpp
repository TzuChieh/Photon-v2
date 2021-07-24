#include "Actor/Material/SurfaceMaterial.h"
#include "Common/assertion.h"
#include "Core/SurfaceBehavior/SurfaceBehavior.h"
#include "Core/Intersectable/PrimitiveMetadata.h"

namespace ph
{

SurfaceMaterial::SurfaceMaterial() = default;

void SurfaceMaterial::genBehaviors(
	ActorCookingContext& ctx,
	PrimitiveMetadata&   metadata) const
{
	genSurface(ctx, metadata.getSurface());
}

}// end namespace ph
