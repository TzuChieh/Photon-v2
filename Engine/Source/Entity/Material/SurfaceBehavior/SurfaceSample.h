#pragma once

#include "Math/Vector3f.h"
#include "Entity/Material/SurfaceBehavior/ESurfaceSampleType.h"

namespace ph
{

class SurfaceSample final
{
public:
	Vector3f           m_LiWeight;
	Vector3f           m_emittedRadiance;
	Vector3f           m_direction;
	ESurfaceSampleType m_type;
};

}// end namespace ph