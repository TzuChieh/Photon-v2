#include "Core/Intersectable/DataStructure/TIndexedPolygonBuffer.h"

namespace ph
{

template<std::size_t N>
inline TIndexedPolygonBuffer<N>::TIndexedPolygonBuffer()
	: m_vertexBuffer()
	, m_indexBuffer()
{}

template<std::size_t N>
inline std::array<math::Vector3R, N> TIndexedPolygonBuffer<N>::getPositions(const std::size_t faceIndex) const
{
	return getFaceAttribute(EVertexAttribute::Position_0, faceIndex);
}

template<std::size_t N>
inline std::array<math::Vector3R, N> TIndexedPolygonBuffer<N>::getTexCoords(const std::size_t faceIndex) const
{
	return getFaceAttribute(EVertexAttribute::TexCoord_0, faceIndex);
}

template<std::size_t N>
inline std::array<math::Vector3R, N> TIndexedPolygonBuffer<N>::getNormals(const std::size_t faceIndex) const
{
	return getFaceAttribute(EVertexAttribute::Normal_0, faceIndex);
}

template<std::size_t N>
inline std::array<math::Vector3R, N> TIndexedPolygonBuffer<N>::getFaceAttribute(
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
inline std::size_t TIndexedPolygonBuffer<N>::numFaces() const
{
	PH_ASSERT_EQ(m_indexBuffer.numUInts() % N, 0);
	return m_indexBuffer.numUInts() / N;
}

template<std::size_t N>
inline bool TIndexedPolygonBuffer<N>::hasTexCoord() const
{
	return hasFaceAttribute(EVertexAttribute::TexCoord_0);
}

template<std::size_t N>
inline bool TIndexedPolygonBuffer<N>::hasNormal() const
{
	return hasFaceAttribute(EVertexAttribute::Normal_0);
}

template<std::size_t N>
inline bool TIndexedPolygonBuffer<N>::hasFaceAttribute(const EVertexAttribute attribute) const
{
	return m_vertexBuffer.hasAttribute(attribute);
}

template<std::size_t N>
inline IndexedVertexBuffer& TIndexedPolygonBuffer<N>::getVertexBuffer()
{
	return m_vertexBuffer;
}

template<std::size_t N>
inline const IndexedVertexBuffer& TIndexedPolygonBuffer<N>::getVertexBuffer() const
{
	return m_vertexBuffer;
}

template<std::size_t N>
inline IndexedUIntBuffer& TIndexedPolygonBuffer<N>::getIndexBuffer()
{
	return m_indexBuffer;
}

template<std::size_t N>
inline const IndexedUIntBuffer& TIndexedPolygonBuffer<N>::getIndexBuffer() const
{
	return m_indexBuffer;
}

template<std::size_t N>
inline constexpr std::size_t TIndexedPolygonBuffer<N>::numPolygonVertices()
{
	return N;
}

template<std::size_t N>
inline constexpr bool TIndexedPolygonBuffer<N>::isTriangular()
{
	return numPolygonVertices() == 3;
}

}// end namespace ph
