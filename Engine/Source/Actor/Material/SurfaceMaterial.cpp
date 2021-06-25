#include "Actor/Material/SurfaceMaterial.h"
#include "Common/assertion.h"
#include "Core/SurfaceBehavior/SurfaceBehavior.h"
#include "Core/Intersectable/PrimitiveMetadata.h"

namespace ph
{

SurfaceMaterial::SurfaceMaterial() = default;

void SurfaceMaterial::genBehaviors(
	CookingContext&    context, 
	PrimitiveMetadata& metadata) const
{
	genSurface(context, metadata.getSurface());
}

}// end namespace ph
