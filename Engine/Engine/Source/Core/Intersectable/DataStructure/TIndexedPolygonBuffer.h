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
class TIndexedPolygonBuffer final
{
	// We do not consider strange cases such as a digon.
	static_assert(N >= 3);

public:
	TIndexedPolygonBuffer();

	std::array<math::Vector3R, N> getPositions(std::size_t faceIndex) const;
	std::array<math::Vector3R, N> getTexCoords(std::size_t faceIndex) const;
	std::array<math::Vector3R, N> getNormals(std::size_t faceIndex) const;
	std::array<math::Vector3R, N> getFaceAttribute(EVertexAttribute attribute, std::size_t faceIndex) const;
	std::size_t numFaces() const;
	bool hasTexCoord() const;
	bool hasNormal() const;
	bool hasFaceAttribute(EVertexAttribute attribute) const;

	/*!
	@return Number of bytes used by the polygons.
	*/
	std::size_t memoryUsage() const;

	/*!
	@return Number of bytes used by a single polygon.
	*/
	float averagePerPolygonMemoryUsage() const;

	IndexedVertexBuffer& getVertexBuffer();
	const IndexedVertexBuffer& getVertexBuffer() const;
	IndexedUIntBuffer& getIndexBuffer();
	const IndexedUIntBuffer& getIndexBuffer() const;

	static constexpr std::size_t numPolygonVertices();
	static constexpr bool isTriangular();

private:
	IndexedVertexBuffer m_vertexBuffer;
	IndexedUIntBuffer m_indexBuffer;
};

}// end namespace ph

#include "Core/Intersectable/DataStructure/TIndexedPolygonBuffer.ipp"
