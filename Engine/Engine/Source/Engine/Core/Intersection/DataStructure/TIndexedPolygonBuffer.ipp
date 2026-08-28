#include "Engine/Core/Intersection/DataStructure/TIndexedPolygonBuffer.h"

namespace ph
{

template<std::size_t N>
inline TIndexedPolygonBuffer<N>::TIndexedPolygonBuffer()
	: m_attributeBuffer()
	, m_indexBuffer()
{}

template<std::size_t N>
inline std::array<math::Vector3R, N> TIndexedPolygonBuffer<N>::getPositions(const std::size_t faceIndex) const
{
	return getFaceVertexAttributes(EPrimitiveAttribute::Position_0, faceIndex);
}

template<std::size_t N>
inline std::array<math::Vector3R, N> TIndexedPolygonBuffer<N>::getTexCoords(const std::size_t faceIndex) const
{
	return getFaceVertexAttributes(EPrimitiveAttribute::TexCoord_0, faceIndex);
}

template<std::size_t N>
inline std::array<math::Vector3R, N> TIndexedPolygonBuffer<N>::getNormals(const std::size_t faceIndex) const
{
	return getFaceVertexAttributes(EPrimitiveAttribute::Normal_0, faceIndex);
}

template<std::size_t N>
inline math::Vector3R TIndexedPolygonBuffer<N>::getFaceAttribute(
	const EPrimitiveAttribute attribute,
	const std::size_t faceIndex) const
{
	PH_ASSERT_LT(faceIndex, numFaces());
	PH_ASSERT(m_attributeBuffer.getAttributeDomain(attribute) == EAttributeDomain::Face);
	return m_attributeBuffer.getAttribute(attribute, faceIndex);
}

template<std::size_t N>
inline std::array<math::Vector3R, N> TIndexedPolygonBuffer<N>::getFaceVertexAttributes(
	const EPrimitiveAttribute attribute,
	const std::size_t faceIndex) const
{
	PH_ASSERT_LT(faceIndex, numFaces());
	PH_ASSERT(m_attributeBuffer.getAttributeDomain(attribute) == EAttributeDomain::Vertex);
	const auto indices = m_indexBuffer.getUInt<N>(N * faceIndex);
	return m_attributeBuffer.getAttribute(attribute, indices);
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
	return hasAttribute(EPrimitiveAttribute::TexCoord_0);
}

template<std::size_t N>
inline bool TIndexedPolygonBuffer<N>::hasNormal() const
{
	return hasAttribute(EPrimitiveAttribute::Normal_0);
}

template<std::size_t N>
inline bool TIndexedPolygonBuffer<N>::hasAttribute(const EPrimitiveAttribute attribute) const
{
	return m_attributeBuffer.hasAttribute(attribute);
}

template<std::size_t N>
inline EAttributeDomain TIndexedPolygonBuffer<N>::getAttributeDomain(const EPrimitiveAttribute attribute) const
{
	return m_attributeBuffer.getAttributeDomain(attribute);
}

template<std::size_t N>
inline std::size_t TIndexedPolygonBuffer<N>::memoryUsage(const IndexedAttributeBufferWriter& attributeWriter) const
{
	return sizeof(*this) +
		attributeWriter.byteBufferSize() +
		m_indexBuffer.byteBufferSize();
}

template<std::size_t N>
inline float TIndexedPolygonBuffer<N>::averagePerPolygonMemoryUsage(const IndexedAttributeBufferWriter& attributeWriter) const
{
	const std::size_t numPolygons = numFaces();
	if(numPolygons == 0)
	{
		return 0.0f;
	}

	return static_cast<float>(static_cast<double>(memoryUsage(attributeWriter)) / numPolygons);
}

template<std::size_t N>
inline IndexedAttributeBuffer& TIndexedPolygonBuffer<N>::getAttributeBuffer()
{
	return m_attributeBuffer;
}

template<std::size_t N>
inline const IndexedAttributeBuffer& TIndexedPolygonBuffer<N>::getAttributeBuffer() const
{
	return m_attributeBuffer;
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
