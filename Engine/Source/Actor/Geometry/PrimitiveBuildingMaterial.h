#pragma once

#include "Common/assertion.h"

namespace ph
{

class PrimitiveMetadata;

class PrimitiveBuildingMaterial final
{
public:
	const PrimitiveMetadata* metadata;

	inline PrimitiveBuildingMaterial(const PrimitiveMetadata* metadata) : 
		metadata(metadata)
	{
		PH_ASSERT(metadata != nullptr);
	}
};

}// end namespace ph