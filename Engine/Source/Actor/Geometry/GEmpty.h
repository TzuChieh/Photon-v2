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

	void genPrimitive(
		const PrimitiveBuildingMaterial&         data, 
		std::vector<std::unique_ptr<Primitive>>& out_primitives) const override;
};

}// end namespace ph
