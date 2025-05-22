#pragma once

#include "Engine/Actor/Material/Material.h"

namespace ph
{

class VolumeMaterial : public Material
{
public:
	void storeCooked(
		CookedMaterial& out_material,
		const CookingContext& ctx) const override = 0;
};

}// end namespace ph
