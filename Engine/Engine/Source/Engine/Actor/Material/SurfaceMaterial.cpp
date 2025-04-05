#include "Engine/Actor/Material/SurfaceMaterial.h"
#include "Engine/Core/SurfaceBehavior/SurfaceBehavior.h"
#include "Engine/Core/Intersection/PrimitiveMetadata.h"

namespace ph
{

void SurfaceMaterial::genBehaviors(
	const CookingContext& ctx,
	PrimitiveMetadata& metadata) const
{
	genSurface(ctx, metadata.getSurface());
}

}// end namespace ph
