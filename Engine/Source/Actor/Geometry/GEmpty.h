#pragma once

#include "Actor/Geometry/Geometry.h"

#include <vector>
#include <memory>

namespace ph
{

class GEmpty : public Geometry
{
public:
	GEmpty();

	void genPrimitive(
		const PrimitiveBuildingMaterial&         data, 
		std::vector<std::unique_ptr<Primitive>>& out_primitives) const override;
};

// In-header Implementations:

inline GEmpty::GEmpty() = default;

}// end namespace ph