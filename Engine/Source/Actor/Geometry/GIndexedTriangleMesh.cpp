#include "Actor/Geometry/GIndexedTriangleMesh.h"
#include "DataIO/PlyFile.h"
#include "Actor/actor_exceptions.h"
#include "DataIO/io_exceptions.h"

namespace ph
{

void GIndexedTriangleMesh::genPrimitive(
	const PrimitiveBuildingMaterial& data,
	std::vector<std::unique_ptr<Primitive>>& out_primitives) const
{

}

}// end namespace ph
