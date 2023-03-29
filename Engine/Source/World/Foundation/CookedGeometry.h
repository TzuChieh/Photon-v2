#pragma once

#include "Core/Intersectable/data_structure_fwd.h"

#include <vector>

namespace ph
{

class Primitive;

class GeometryCookConfig final
{
public:
	bool preferTriangulated = false;
	bool preferIndexedVertices = false;

	/*! Force triangulated result. Generally this is handled by the base `Geometry` class and do
	not need to be explicitly handled in the derived classes. It is an error if triangulation 
	is not possible.
	*/
	bool forceTriangulated = false;
};

class CookedGeometry final
{
public:
	/*! Primitives that form the geometry. */
	std::vector<const Primitive*> primitives;

	/*! An alternative representation of the geometry. May not always be available. */
	const IndexedTriangleBuffer* triangleBuffer = nullptr;
};

}// end namespace ph
