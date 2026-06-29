#include "Engine/Actor/Material/Material.h"
#include "Engine/World/Foundation/CookedMaterial.h"
#include "Engine/World/Foundation/CookingContext.h"

namespace ph
{

void Material::cook(const CookingContext& ctx, CookedMaterial& out_material) const
{
	storeCooked(ctx, out_material);
}

}// end namespace ph
