#pragma once

#include "Actor/Geometry/Geometry.h"

#include <vector>
#include <memory>

namespace ph
{

class GEmpty : public Geometry
{
public:
	GEmpty() = default;

	void storeCooked(
		CookedGeometry& out_geometry,
		const CookingContext& ctx) const override;

	void genPrimitive(
		const PrimitiveBuildingMaterial&         data, 
		std::vector<std::unique_ptr<Primitive>>& out_primitives) const override;
};

}// end namespace ph
