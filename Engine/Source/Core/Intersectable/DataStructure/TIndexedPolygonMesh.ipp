#include "Core/Intersectable/DataStructure/TIndexedPolygonMesh.h"

namespace ph
{

template<std::size_t N>
inline TIndexedPolygonMesh<N>::TIndexedPolygonMesh()
	: m_vertexBuffer()
	, m_indexBuffer()
{}

template<std::size_t N>
inline std::array<math::Vector3R, N> TIndexedPolygonMesh<N>::getPositions(const std::size_t faceIndex) const
{
	return getFaceAttribute(EVertexAttribute::Position_0, faceIndex);
}

template<std::size_t N>
inline std::array<math::Vector3R, N> TIndexedPolygonMesh<N>::getFaceAttribute(
	const EVertexAttribute attribute,
	const std::size_t      faceIndex) const
{
	PH_ASSERT_LT(faceIndex, numFaces());

	// Fetch attributes from the buffer. 
	// (loop should be optimized by the compiler)
	std::array<math::Vector3R, N> result;
	for(std::size_t i = 0; i < N; ++i)
	{
		result[i] = m_vertexBuffer.getAttribute(attribute, m_indexBuffer.getUInt(N * faceIndex + i));
	}
	return result;
}

template<std::size_t N>
inline std::size_t TIndexedPolygonMesh<N>::numFaces() const
{
	PH_ASSERT_EQ(m_indexBuffer.numUInts() % N, 0);
	return m_indexBuffer.numUInts() / N;
}

template<std::size_t N>
inline IndexedVertexBuffer& TIndexedPolygonMesh<N>::getVertexBuffer()
{
	return m_vertexBuffer;
}

template<std::size_t N>
inline const IndexedVertexBuffer& TIndexedPolygonMesh<N>::getVertexBuffer() const
{
	return m_vertexBuffer;
}

template<std::size_t N>
inline IndexedUIntBuffer& TIndexedPolygonMesh<N>::getIndexBuffer()
{
	return m_indexBuffer;
}

template<std::size_t N>
inline const IndexedUIntBuffer& TIndexedPolygonMesh<N>::getIndexBuffer() const
{
	return m_indexBuffer;
}

}// end namespace ph
