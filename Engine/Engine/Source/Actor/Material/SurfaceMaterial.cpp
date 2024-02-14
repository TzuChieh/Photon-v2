#include "Actor/Material/SurfaceMaterial.h"
#include "Core/SurfaceBehavior/SurfaceBehavior.h"
#include "Core/Intersection/PrimitiveMetadata.h"

namespace ph
{

void SurfaceMaterial::genBehaviors(
	const CookingContext& ctx,
	PrimitiveMetadata& metadata) const
{
	genSurface(ctx, metadata.getSurface());
}

}// end namespace ph
