#pragma once

#include "Core/Intersectable/data_structure_fwd.h"

#include <vector>

namespace ph
{

class Primitive;

class CookedGeometry final
{
public:
	/*! Primitives that form the geometry. */
	std::vector<const Primitive*> primitives;

	/*! An alternative representation of the geometry. May not always be available. */
	const IndexedTriangleBuffer* triangleBuffer = nullptr;
};

}// end namespace ph
