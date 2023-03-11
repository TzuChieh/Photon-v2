#pragma once

#include "Common/primitive_type.h"
#include "Common/assertion.h"
#include "Math/TVector2.h"
#include "Math/TVector3.h"
#include "Utility/utility.h"

#include <memory>
#include <cstddef>
#include <limits>
#include <array>
#include <climits>
#include <type_traits>

namespace ph
{

enum class EVertexAttribute : uint8
{
	Position_0 = 0,
	Normal_0,
	Tangent_0,
	TexCoord_0,
	TexCoord_1,
	Color_0,

	// Special values
	SIZE
};

enum class EVertexElement : uint8
{
	Float32 = 0,
	Float16,
	Int32,
	Int16,
	OctahedralUnitVec3_32,
	OctahedralUnitVec3_24,

	// Special values
	SIZE
};

/*! @brief A general vertex buffer for storing various indexed attributes.
*/
class IndexedVertexBuffer final
{
	static_assert(sizeof(std::byte) * CHAR_BIT == 8,
		"The buffer explicitly depends on the fact that std::byte contains 8 bits.");

public:
	IndexedVertexBuffer();

	/*! @brief Declares a vertex attribute with default layout (AoS).
	*/
	void declareAttribute(
		EVertexAttribute attribute,
		EVertexElement element,
		std::size_t numElements,
		bool shouldNormalize = false);

	/*! @brief Declares a vertex attribute with custom layout.
	*/
	void declareAttribute(
		EVertexAttribute attribute,
		EVertexElement element,
		std::size_t numElements,
		std::size_t strideOffset,
		std::size_t strideSize,
		bool shouldNormalize = false);

	void allocate(std::size_t numVertices);

	void setAttribute(EVertexAttribute attribute, std::size_t index, const math::Vector3R& value);
	void setAttribute(EVertexAttribute attribute, std::size_t index, const math::Vector2R& value);
	void setAttribute(EVertexAttribute attribute, std::size_t index, real value);
	void setVertices(const std::byte* srcBytes, std::size_t numBytes, std::size_t dstOffset = 0);
	math::Vector3R getAttribute(EVertexAttribute attribute, std::size_t index) const;
	std::size_t estimateMemoryUsage() const;
	bool isAllocated() const;
	std::size_t numVertices() const;

	/*! @brief Access to the underlying raw byte buffer.
	*/
	///@{
	std::byte* getData();
	const std::byte* getData() const;
	///@}

	/*! @brief Info for a declared vertex attribute.
	*/
	struct AttributeDeclaration final
	{
		std::size_t strideOffset;
		std::size_t strideSize;
		EVertexElement element;
		uint8 numElements : 2;
		uint8 shouldNormalize : 1;

		AttributeDeclaration();

		bool isEmpty() const;
	};

	/*! @brief Get information for a previously declared attribute.
	Can only be called after allocation.
	*/
	AttributeDeclaration getAttributeDeclaration(EVertexAttribute attribute) const;

private:
	// Sizes are in bytes

	// Internal info for a vertex attribute. Members are ordered to minimize padding.
	struct Entry final
	{
		inline constexpr static auto INVALID_STRIDE_VALUE = static_cast<std::size_t>(-1);

		union
		{
			/*! @brief Pointer to the beginning of the attribute data. 
			This is the only valid member after `allocate()`.
			*/
			std::byte* u_attributeBuffer;

			/*! @brief Offset to the beginning of the attribute data in bytes. 
			Valid during the declaration of vertex layout only.
			*/
			std::size_t u_strideOffset;
		};

		/*! @brief Number of bytes to offset to get the next attribute. */
		std::size_t strideSize;

		EVertexElement element;

		/*! @brief Number of elements in this entry. Expected to be within [1, 3]. */
		uint8 numElements : 2;

		/*! @brief Whether the stored value is in [0, 1] ([-1, 1] for signed types). 
		This attribute is for integral types only. Take uint8 for example, if this attribute is true, 
		an input value of 255 will be converted to 1.0 on load; otherwise, the value is converted to 
		real as-is (i.e., 255 becomes 255.0).
		*/
		uint8 shouldNormalize : 1;

		Entry();

		/*! @brief Whether the attribute is not used. */
		bool isEmpty() const;

		bool hasStrideInfo() const;
	};

	bool hasEntry(EVertexAttribute attribute) const;
	const Entry& getEntry(EVertexAttribute attribute) const;
	void ensureConsistentVertexLayout() const;

	inline constexpr static auto MAX_ENTRIES = enum_size<EVertexAttribute>();

	std::array<std::underlying_type_t<EVertexAttribute>, MAX_ENTRIES> m_attributeTypeToEntryIndex;
	std::array<Entry, MAX_ENTRIES> m_entries;
	std::underlying_type_t<EVertexAttribute> m_numEntries;

	std::unique_ptr<std::byte[]> m_byteBuffer;
	std::size_t m_byteBufferSize;
	uint16 m_vertexSize;
};

// In-header Implementations:

inline std::size_t IndexedVertexBuffer::estimateMemoryUsage() const
{
	return sizeof(*this) + m_byteBufferSize;
}

inline bool IndexedVertexBuffer::Entry::isEmpty() const
{
	return numElements == 0;
}

inline bool IndexedVertexBuffer::Entry::hasStrideInfo() const
{
	// Cannot have partially filled stride info
	PH_ASSERT(
		(u_strideOffset != INVALID_STRIDE_VALUE && strideSize != INVALID_STRIDE_VALUE) ||
		(u_strideOffset == INVALID_STRIDE_VALUE && strideSize == INVALID_STRIDE_VALUE));

	return u_strideOffset != INVALID_STRIDE_VALUE && strideSize != INVALID_STRIDE_VALUE;
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

inline bool IndexedVertexBuffer::hasEntry(const EVertexAttribute attribute) const
{
	return m_attributeTypeToEntryIndex[enum_to_value(attribute)] != MAX_ENTRIES;
}

inline auto IndexedVertexBuffer::getEntry(const EVertexAttribute attribute) const
-> const Entry&
{
	const auto entryIndex = m_attributeTypeToEntryIndex[enum_to_value(attribute)];
	PH_ASSERT_LT(entryIndex, m_numEntries);
	return m_entries[entryIndex];
}

inline std::byte* IndexedVertexBuffer::getData()
{
	PH_ASSERT(isAllocated());
	return m_byteBuffer.get();
}

inline const std::byte* IndexedVertexBuffer::getData() const
{
	PH_ASSERT(isAllocated());
	return m_byteBuffer.get();
}

inline bool IndexedVertexBuffer::AttributeDeclaration::isEmpty() const
{
	return numElements == 0;
}

}// end namespace ph
