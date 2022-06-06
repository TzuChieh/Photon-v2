#pragma once

#include "World/Foundation/TUniquePtrCollection.h"
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
	TUniquePtrCollection<PrimitiveMetadata> m_metadatas;
	TUniquePtrCollection<math::Transform>   m_transforms;

	std::vector<ResourceCookReport> m_cookReports;
};

}// end namespace ph
