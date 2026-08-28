#pragma once

#include "Engine/Core/Intersection/data_structure_fwd.h"
#include "Engine/Core/Intersection/DataStructure/IndexedAttributeBuffer.h"
#include "Engine/Core/Intersection/DataStructure/IndexedUIntBuffer.h"
#include "Engine/Math/TVector3.h"

#include <Common/assertion.h>

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
	math::Vector3R getFaceAttribute(EPrimitiveAttribute attribute, std::size_t faceIndex) const;
	std::array<math::Vector3R, N> getFaceVertexAttributes(EPrimitiveAttribute attribute, std::size_t faceIndex) const;
	std::size_t numFaces() const;
	bool hasTexCoord() const;
	bool hasNormal() const;
	bool hasAttribute(EPrimitiveAttribute attribute) const;
	EAttributeDomain getAttributeDomain(EPrimitiveAttribute attribute) const;

	/*! @brief Get the total memory used by this polygon buffer.
	@param attributeWriter Writer returned when allocating this buffer's attributes.
	*/
	std::size_t memoryUsage(const IndexedAttributeBufferWriter& attributeWriter) const;

	/*! @brief Get the average memory used by a single polygon.
	@param attributeWriter Writer returned when allocating this buffer's attributes.
	*/
	float averagePerPolygonMemoryUsage(const IndexedAttributeBufferWriter& attributeWriter) const;

	IndexedAttributeBuffer& getAttributeBuffer();
	const IndexedAttributeBuffer& getAttributeBuffer() const;
	IndexedUIntBuffer& getIndexBuffer();
	const IndexedUIntBuffer& getIndexBuffer() const;

	static constexpr std::size_t numPolygonVertices();
	static constexpr bool isTriangular();

private:
	IndexedAttributeBuffer m_attributeBuffer;
	IndexedUIntBuffer m_indexBuffer;
};

}// end namespace ph

#include "Engine/Core/Intersection/DataStructure/TIndexedPolygonBuffer.ipp"
