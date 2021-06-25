#pragma once

#include "Actor/Material/SurfaceMaterial.h"

#include <memory>

namespace ph
{

class AbradedSurface : public SurfaceMaterial
{
public:
	void genSurface(CookingContext& context, SurfaceBehavior& behavior) const override = 0;

private:

};

}// end namespace ph
