#pragma once

#include "Utility/INoCopyAndMove.h"
#include "Utility/TUniquePtrVector.h"
#include "Core/Intersectable/PrimitiveMetadata.h"
#include "Math/Transform/Transform.h"
#include "Utility/Concurrent/TSynchronized.h"

#include <vector>

namespace ph
{

class CookedResourceCollection final : private INoCopyAndMove
{
public:

private:
	TUniquePtrVector<PrimitiveMetadata> m_metadatas;
	TUniquePtrVector<math::Transform> m_transforms;
};

}// end namespace ph
