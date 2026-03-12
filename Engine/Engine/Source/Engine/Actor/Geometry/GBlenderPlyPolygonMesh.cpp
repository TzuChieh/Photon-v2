#include "Engine/Actor/Geometry/GBlenderPlyPolygonMesh.h"
#include "Engine/DataIO/PlyFile.h"

namespace ph
{

void GBlenderPlyPolygonMesh::SdlWritePly::operator () () const
{
	PlyFile ply;
}

void GBlenderPlyPolygonMesh::storeCooked(
	CookedGeometry& out_geometry,
	const CookingContext& ctx) const
{
	// TODO
}

}// end namespace ph
