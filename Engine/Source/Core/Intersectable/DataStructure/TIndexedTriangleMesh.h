#pragma once

#include "Core/Intersectable/DataStructure/IndexedVertexBuffer.h"
#include "Math/TVector3.h"

#include <cstddef>
#include <utility>
#include <memory>
#include <array>

namespace ph
{

template<typename Index>
class TIndexedTriangleMesh final
{
public:
	TIndexedTriangleMesh();

	std::array<math::Vector3R, 3> getFaceAttribute(EVertexAttribute attribute, std::size_t faceIndex) const;

	// TODO: getFace()
	// TODO: numFaces()

	void setFaceIndices(Index* indices, std::size_t numIndices);
	IndexedVertexBuffer& getVertexBuffer();
	const IndexedVertexBuffer& getVertexBuffer() const;

private:
	IndexedVertexBuffer m_vertexBuffer;
};

// In-header Implementations:

template<typename Index>
inline IndexedVertexBuffer& TIndexedTriangleMesh<Index>::getVertexBuffer()
{
	return m_vertexBuffer;
}

template<typename Index>
inline const IndexedVertexBuffer& TIndexedTriangleMesh<Index>::getVertexBuffer() const
{
	return m_vertexBuffer;
}

}// end namespace ph
