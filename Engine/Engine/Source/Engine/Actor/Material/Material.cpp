#include "Engine/Actor/Material/Material.h"
#include "Engine/World/Foundation/CookedMaterial.h"
#include "Engine/World/Foundation/CookingContext.h"

#include <utility>

namespace ph
{

void Material::cook(const CookingContext& ctx, CookedMaterial& out_material) const
{
	storeCooked(ctx, out_material);
	
	out_material.interfaceMask = m_interfaceMask
		? m_interfaceMask->genRealTexture(ctx)
		: nullptr;
}

void Material::setInterfaceMask(std::shared_ptr<Image> interfaceMask)
{
	m_interfaceMask = std::move(interfaceMask);
}

}// end namespace ph
