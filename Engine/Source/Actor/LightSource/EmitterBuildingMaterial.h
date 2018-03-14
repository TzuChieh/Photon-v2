#pragma once

namespace ph
{

class Primitive;

class EmitterBuildingMaterial final
{
public:
	const Primitive* primitive;

	inline EmitterBuildingMaterial() : 
		primitive(nullptr)
	{}
};

}// end namespace ph