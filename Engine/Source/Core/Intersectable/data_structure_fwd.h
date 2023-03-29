#pragma once

#include <cstddef>

namespace ph
{

template<std::size_t N>
class TIndexedPolygonBuffer;

using IndexedTriangleBuffer = TIndexedPolygonBuffer<3>;
using IndexedQuadBuffer = TIndexedPolygonBuffer<4>;

}// end namespace ph
