#pragma once

#include "Engine/Actor/Geometry/Geometry.h"

#include <vector>
#include <memory>

namespace ph
{

class GEmpty : public Geometry
{
public:
	GEmpty() = default;

	void storeCooked(
		const CookingContext& ctx,
		CookedGeometry& out_geometry) const override;
};

}// end namespace ph
