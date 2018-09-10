#include "Actor/Geometry/GEmpty.h"
#include "Core/Intersectable/PEmpty.h"
#include "Actor/Geometry/PrimitiveBuildingMaterial.h"

namespace ph
{

void GEmpty::genPrimitive(
	const PrimitiveBuildingMaterial&         data,
	std::vector<std::unique_ptr<Primitive>>& out_primitives) const
{
	out_primitives.push_back(std::make_unique<PEmpty>(data.metadata));
}

}// end namespace ph