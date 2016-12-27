#pragma once

#include "Core/SurfaceBehavior/SurfaceBehavior.h"
#include "Math/Transform.h"

namespace ph
{

class PrimitiveMetadata final
{
public:
	SurfaceBehavior surfaceBehavior;
	Transform       localToWorld;
	Transform       worldToLocal;

	PrimitiveMetadata();
};

}// end namespace ph