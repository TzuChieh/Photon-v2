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
		CookedGeometry& out_geometry,
		const CookingContext& ctx) const override;
};

}// end namespace ph
