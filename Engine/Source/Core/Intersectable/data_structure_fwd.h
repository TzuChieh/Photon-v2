#pragma once

#include <cstddef>

namespace ph
{

template<std::size_t N>
class TIndexedPolygonMesh;

using IndexedTriangleMesh = TIndexedPolygonMesh<3>;
using IndexedQuadMesh = TIndexedPolygonMesh<4>;

}// end namespace ph
