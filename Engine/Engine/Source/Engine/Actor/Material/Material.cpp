#include "Engine/Actor/Material/Material.h"
#include "Engine/World/Foundation/CookedMaterial.h"
#include "Engine/World/Foundation/CookingContext.h"
#include "Engine/World/Foundation/CookedResourceCollection.h"

#include <Common/assertion.h>

namespace ph
{

CookedMaterial* Material::createCooked(const CookingContext& ctx) const
{
	CookedMaterial* cookedMaterial = ctx.getResources()->makeMaterial(getId());
	PH_ASSERT(cookedMaterial);

	storeCooked(*cookedMaterial, ctx);

	return cookedMaterial;
}

}// end namespace ph
