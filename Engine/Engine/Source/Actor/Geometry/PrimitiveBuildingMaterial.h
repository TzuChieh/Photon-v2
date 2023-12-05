#pragma once

#include <Common/assertion.h>

namespace ph
{

class PrimitiveMetadata;

class PrimitiveBuildingMaterial final
{
public:
	PrimitiveMetadata* metadata;

	inline PrimitiveBuildingMaterial(PrimitiveMetadata* const metadata) : 
		metadata(metadata)
	{
		PH_ASSERT(metadata != nullptr);
	}
};

}// end namespace ph
