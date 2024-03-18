#include "Actor/Geometry/GIndexedTriangleMesh.h"
#include "DataIO/PlyFile.h"
#include "Actor/Basic/exceptions.h"

#include <Common/io_exceptions.h>

namespace ph
{

void GIndexedTriangleMesh::genPrimitive(
	const PrimitiveBuildingMaterial& data,
	std::vector<std::unique_ptr<Primitive>>& out_primitives) const
{
	// TODO
	PH_ASSERT_UNREACHABLE_SECTION();
}

}// end namespace ph
