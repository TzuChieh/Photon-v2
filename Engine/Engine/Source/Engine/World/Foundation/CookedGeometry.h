#pragma once

#include "Engine/Core/Intersection/GeometryInfo.h"
#include "Engine/Core/Intersection/data_structure_fwd.h"
#include "Engine/Core/Intersection/DataStructure/TIndexRangeMap.h"

#include <vector>

namespace ph
{

class Primitive;

class CookedGeometry final
{
public:
	/*! Primitives that form the geometry. */
	std::vector<const Primitive*> primitives;

	/*! Geometry-wide information in the primitives' local coordinate system. */
	GeometryInfo geometryInfo;

	/*! An alternative representation of the geometry. May not always be available and may not cover 
	all the shapes defined by `primitives`.
	*/
	const IndexedTriangleBuffer* triangleView = nullptr;

	TIndexRangeMap<uint64, uint32> faceIdToMetadataSlot;

	/*! Whether a baked transform reversed the stored vertex winding. */
	bool isWindingFlipped = false;
};

}// end namespace ph
