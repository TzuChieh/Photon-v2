#pragma once

#include "Core/SurfaceBehavior/SurfaceBehavior.h"
#include "Math/Transform.h"

namespace ph
{

class PrimitiveMetadata final
{
public:
	SurfaceBehavior* m_surfaceBehavior;
	Transform*       m_localToWorld;
	Transform*       m_worldToLocal;

	PrimitiveMetadata();
};

}// end namespace ph