#pragma once

namespace ph
{

class PrimitiveMetadata;

class PrimitiveBuildingMaterial final
{
public:
	const PrimitiveMetadata* metadata;

	inline PrimitiveBuildingMaterial() : 
		metadata(nullptr)
	{

	}
};

}// end namespace ph