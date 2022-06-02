#pragma once

#include "Actor/Foundation/TUniquePtrCollection.h"
#include "Core/Intersectable/PrimitiveMetadata.h"
#include "Math/Transform/Transform.h"

namespace ph
{

class PreCookResourceCollection final
{
public:

private:
	TUniquePtrCollection<PrimitiveMetadata> m_metadatas;
	TUniquePtrCollection<math::Transform>   m_transforms;
};

}// end namespace ph
