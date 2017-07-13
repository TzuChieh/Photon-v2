#pragma once

#include "Core/SurfaceBehavior/SurfaceBehavior.h"
#include "Math/Transform/StaticTransform.h"

namespace ph
{

class PrimitiveMetadata final
{
public:
	SurfaceBehavior surfaceBehavior;
	//StaticTransform localToWorld;
	//StaticTransform worldToLocal;

	PrimitiveMetadata();
};

}// end namespace ph