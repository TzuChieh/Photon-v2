#include "Engine/Actor/Geometry/GIndexedTriangleMesh.h"
#include "Engine/DataIO/PlyFile.h"
#include "Engine/Actor/Basic/exceptions.h"

#include <Common/io_exceptions.h>

namespace ph
{

void GIndexedTriangleMesh::storeCooked(
	CookedGeometry& out_geometry,
	const CookingContext& ctx) const
{
	// TODO
	PH_ASSERT_UNREACHABLE_SECTION();
}

}// end namespace ph
