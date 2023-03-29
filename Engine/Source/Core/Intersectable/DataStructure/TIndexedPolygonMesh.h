#pragma once

#include "Core/Intersectable/data_structure_fwd.h"
#include "Core/Intersectable/DataStructure/IndexedVertexBuffer.h"
#include "Core/Intersectable/DataStructure/IndexedUIntBuffer.h"
#include "Math/TVector3.h"
#include "Common/assertion.h"

#include <cstddef>
#include <utility>
#include <memory>
#include <array>

namespace ph
{

/*!
@tparam N Number of polygon vertices.
*/
template<std::size_t N>
class TIndexedPolygonMesh final
{
	// We do not consider strange cases such as a digon.
	static_assert(N >= 3);

public:
	TIndexedPolygonMesh();

	std::array<math::Vector3R, N> getPositions(std::size_t faceIndex) const;
	std::array<math::Vector3R, N> getFaceAttribute(EVertexAttribute attribute, std::size_t faceIndex) const;
	std::size_t numFaces() const;

	IndexedVertexBuffer& getVertexBuffer();
	const IndexedVertexBuffer& getVertexBuffer() const;
	IndexedUIntBuffer& getIndexBuffer();
	const IndexedUIntBuffer& getIndexBuffer() const;

private:
	IndexedVertexBuffer m_vertexBuffer;
	IndexedUIntBuffer   m_indexBuffer;
};

}// end namespace ph

#include "Core/Intersectable/DataStructure/TIndexedPolygonMesh.ipp"
