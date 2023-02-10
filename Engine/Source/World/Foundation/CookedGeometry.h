#pragma once

#include <vector>

namespace ph
{

class Primitive;

class GeometryCookConfig final
{
public:
	bool preferTriangulated = false;
	bool forceTriangulated = false;
};

class CookedGeometry final
{
public:
	std::vector<const Primitive*> primitives;
};

}// end namespace ph
