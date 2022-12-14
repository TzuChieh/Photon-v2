#pragma once

#include "Common/primitive_type.h"
#include "Common/assertion.h"
#include "Math/TVector2.h"
#include "Math/TVector3.h"

#include <memory>
#include <cstddef>
#include <limits>
#include <array>
#include <climits>

namespace ph
{

enum class EVertexAttribute
{
	Position_0 = 0,
	Normal_0,
	Tangent_0,
	TexCoord_0,
	TexCoord_1,
	Color_0,

	// Special values
	NUM
};

enum class EVertexElement
{
	Float32 = 0,
	Float16,
	Int32,
	Int16,
	OctahedralUnitVec3_32,
	OctahedralUnitVec3_24,

	// Special values
	NUM
};

/*! @brief A general vertex buffer for storing various indexed attributes.
*/
class IndexedVertexBuffer final
{
	static_assert(sizeof(std::byte) * CHAR_BIT == 8,
		"The buffer explicitly depends on the fact that std::byte contains 8 bits.");

public:
	IndexedVertexBuffer();

	void setEntry(
		EVertexAttribute attribute,
		EVertexElement   element,
		std::size_t      numElements,
		bool             shouldNormalize = false);

	void allocate(std::size_t numVertices);

	void setAttribute(EVertexAttribute attribute, std::size_t index, const math::Vector3R& value);
	void setAttribute(EVertexAttribute attribute, std::size_t index, const math::Vector2R& value);
	void setAttribute(EVertexAttribute attribute, std::size_t index, real value);
	void setVertices(const std::byte* srcBytes, std::size_t numBytes, std::size_t dstOffset = 0);
	math::Vector3R getAttribute(EVertexAttribute attribute, std::size_t index) const;
	std::size_t estimateMemoryUsage() const;
	bool isAllocated() const;
	std::size_t numVertices() const;

private:
	// Sizes are in bytes

	// Info for a vertex attribute. Members are ordered to minimize padding.
	struct Entry final
	{
		inline constexpr static std::size_t INVALID_STRIZE_SIZE;

		/*! @brief Pointer to the beginning of the attribute data. */
		std::byte* attributeBuffer;

		/*! @brief Number of bytes to offset to get the next attribute. */
		std::size_t strideSize;

		EVertexElement element;

		/*! @brief Number of elements in this entry. Expected to be within [1, 3]. */
		uint8 numElements : 2;

		/*! @brief Whether the stored value is in [0, 1] ([-1, 1] for signed types). 
		This attribute is for integral types only. Take uint8 for example, if this attribute is true, an
		input value of 255 will be converted to 1.0 on load; otherwise, the value is converted to real
		as-is (i.e., 255 becomes 255.0).
		*/
		uint8 shouldNormalize : 1;

		Entry();

		/*! @brief Whether the attribute is not used. */
		bool isEmpty() const;
	};

	std::array<Entry, static_cast<std::size_t>(EVertexAttribute::NUM)> m_entries;

	std::unique_ptr<std::byte[]> m_byteBuffer;
	std::size_t                  m_byteBufferSize;
	uint32f                      m_vertexSize;
};

// In-header Implementations:

inline std::size_t IndexedVertexBuffer::estimateMemoryUsage() const
{
	return sizeof(IndexedVertexBuffer) + m_byteBufferSize;
}

inline bool IndexedVertexBuffer::Entry::isEmpty() const
{
	return numElements == 0;
}

inline bool IndexedVertexBuffer::isAllocated() const
{
	return m_byteBuffer != nullptr;
}

inline std::size_t IndexedVertexBuffer::numVertices() const
{
	return m_vertexSize > 0 ? m_byteBufferSize / m_vertexSize : 0;
}

inline void IndexedVertexBuffer::setAttribute(const EVertexAttribute attribute, const std::size_t index, const math::Vector2R& value)
{
	setAttribute(attribute, index, math::Vector3R(value[0], value[1], 0.0_r));
}

inline void IndexedVertexBuffer::setAttribute(const EVertexAttribute attribute, const std::size_t index, real value)
{
	setAttribute(attribute, index, math::Vector3R(value, 0.0_r, 0.0_r));
}

}// end namespace ph
