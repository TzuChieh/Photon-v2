#pragma once

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

class IndexedTriangleMesh final
{
public:
	IndexedTriangleMesh();

	std::array<math::Vector3R, 3> getPositions(std::size_t faceIndex) const;
	std::array<math::Vector3R, 3> getFaceAttribute(EVertexAttribute attribute, std::size_t faceIndex) const;
	std::size_t numFaces() const;

	IndexedVertexBuffer& getVertexBuffer();
	const IndexedVertexBuffer& getVertexBuffer() const;
	IndexedUIntBuffer& getIndexBuffer();
	const IndexedUIntBuffer& getIndexBuffer() const;

private:
	IndexedVertexBuffer m_vertexBuffer;
	IndexedUIntBuffer   m_indexBuffer;
};

// In-header Implementations:

inline std::array<math::Vector3R, 3> IndexedTriangleMesh::getPositions(const std::size_t faceIndex) const
{
	return getFaceAttribute(EVertexAttribute::Position_0, faceIndex);
}

inline std::array<math::Vector3R, 3> IndexedTriangleMesh::getFaceAttribute(
	const EVertexAttribute attribute,
	const std::size_t      faceIndex) const
{
	PH_ASSERT_LT(faceIndex, numFaces());

	return
	{
		m_vertexBuffer.getAttribute(attribute, m_indexBuffer.getUInt(3 * faceIndex)),
		m_vertexBuffer.getAttribute(attribute, m_indexBuffer.getUInt(3 * faceIndex + 1)),
		m_vertexBuffer.getAttribute(attribute, m_indexBuffer.getUInt(3 * faceIndex + 2))
	};
}

inline std::size_t IndexedTriangleMesh::numFaces() const
{
	PH_ASSERT_EQ(m_indexBuffer.numUInts() % 3, 0);
	return m_indexBuffer.numUInts() / 3;
}

inline IndexedVertexBuffer& IndexedTriangleMesh::getVertexBuffer()
{
	return m_vertexBuffer;
}

inline const IndexedVertexBuffer& IndexedTriangleMesh::getVertexBuffer() const
{
	return m_vertexBuffer;
}

inline IndexedUIntBuffer& IndexedTriangleMesh::getIndexBuffer()
{
	return m_indexBuffer;
}

inline const IndexedUIntBuffer& IndexedTriangleMesh::getIndexBuffer() const
{
	return m_indexBuffer;
}

}// end namespace ph
