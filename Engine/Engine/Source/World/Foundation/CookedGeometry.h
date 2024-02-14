#pragma once

#include "Core/Intersection/data_structure_fwd.h"

#include <vector>

namespace ph
{

class Primitive;

class CookedGeometry final
{
public:
	/*! Primitives that form the geometry. */
	std::vector<const Primitive*> primitives;

	/*! An alternative representation of the geometry. May not always be available. Will cover 
	all the shapes defined by `primitives` if provided.
	*/
	const IndexedTriangleBuffer* triangleView = nullptr;
};

}// end namespace ph
