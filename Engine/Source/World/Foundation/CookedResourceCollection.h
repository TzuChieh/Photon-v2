#pragma once

#include "Utility/TUniquePtrVector.h"
#include "Core/Intersectable/PrimitiveMetadata.h"
#include "Math/Transform/Transform.h"
#include "World/Foundation/ResourceCookReport.h"

#include <vector>

namespace ph
{

class CookedResourceCollection final
{
public:

private:
	TUniquePtrVector<PrimitiveMetadata> m_metadatas;
	TUniquePtrVector<math::Transform>   m_transforms;

	std::vector<ResourceCookReport> m_cookReports;
};

}// end namespace ph
