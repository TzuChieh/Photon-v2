#pragma once

#include "Engine/Core/Intersection/Intersectable.h"
#include "Engine/Math/math.h"
#include "Engine/Math/TVector2.h"
#include "Engine/Math/TVector3.h"
#include "Engine/Utility/utility.h"

#include <Common/assertion.h>
#include <Common/memory.h>
#include <Common/primitive_type.h>

#include <array>
#include <climits>
#include <concepts>
#include <cstddef>
#include <new>
#include <type_traits>

namespace ph
{

enum class EAttributeElement : uint8
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

class IndexedAttributeBufferWriter;

/*! @brief A general buffer for storing various indexed polygon attributes.
*/
class IndexedAttributeBuffer final
{
	static_assert(sizeof(std::byte) * CHAR_BIT == 8,
		"The buffer explicitly depends on the fact that std::byte contains 8 bits.");

public:
	IndexedAttributeBuffer();

	/*! @brief Declares an attribute with automatic layout (AoS) within its domain.
	AoS: each vertex/face attribute value set forms a struct.
	*/
	void declareAttribute(
		EPrimitiveAttribute attribute,
		EAttributeDomain domain,
		EAttributeElement element,
		std::size_t numElements,
		bool shouldNormalize = false);

	/*! @brief Declares an attribute with custom layout.
	@param attribute Primary type of the attribute.
	@param domain Indexing domain of the attribute.
	@param element Type of the datum that comprises a single attribute.
	@param numElements Number of elements comprising a single attribute value.
	@param strideOffset Offset from the beginning of attribute storage to the attribute.
	@param strideSize The amount of offset to reach the next attribute.
	@param shouldNormalize Whether to map the stored value to [-1, 1]/[0, 1] depending on @p element.
	@note All attributes must consistently use either automatic or custom layout.
	*/
	void declareAttribute(
		EPrimitiveAttribute attribute,
		EAttributeDomain domain,
		EAttributeElement element,
		std::size_t numElements,
		std::size_t strideOffset,
		std::size_t strideSize,
		bool shouldNormalize = false);

	/*! @brief Finalizes the layout and returns a mutable view for initializing the buffer.
	The returned writer must not outlive this buffer.
	*/
	[[nodiscard]]
	IndexedAttributeBufferWriter allocate(
		std::size_t numVertices,
		std::size_t numFaces = 0);

	bool hasAttribute(EPrimitiveAttribute attribute) const;
	EAttributeDomain getAttributeDomain(EPrimitiveAttribute attribute) const;

	math::Vector3R getAttribute(
		EPrimitiveAttribute attribute,
		std::size_t index) const;

	/*! @brief Gather a fixed number of attribute values.
	@param indices Indices in the attribute's declared domain. Values do not need to be contiguous.
	*/
	template<std::size_t N, std::unsigned_integral Index>
	std::array<math::Vector3R, N> getAttribute(
		EPrimitiveAttribute attribute,
		const std::array<Index, N>& indices) const;

	/*! @brief Info for a declared attribute.
	*/
	struct AttributeDeclaration final
	{
		std::size_t strideOffset;
		std::size_t strideSize;
		EAttributeElement element;
		EAttributeDomain domain;
		uint8 numElements : 2;
		uint8 shouldNormalize : 1;

		AttributeDeclaration();

		bool isEmpty() const;
	};

	/*! @brief Get information for a previously declared attribute.
	Can only be called after allocation.
	*/
	AttributeDeclaration getAttributeDeclaration(EPrimitiveAttribute attribute) const;

private:
	friend class IndexedAttributeBufferWriter;

	// Sizes are in bytes

	// Internal info for an attribute. Members are ordered to minimize padding.
	struct Entry final
	{
		inline constexpr static auto INVALID_STRIDE_VALUE = static_cast<std::size_t>(-1);

		union
		{
			/*! @brief Pointer to the first stored value of this attribute.
			This is the only valid member after `allocate()`.
			*/
			std::byte* u_attributeBuffer;

			/*! @brief Offset from attribute storage to the first stored value.
			Valid during attribute layout declaration only.
			*/
			std::size_t u_strideOffset;
		};

		/*! @brief Number of bytes to offset to get the next attribute. */
		std::size_t strideSize;

		EAttributeElement element;
		EAttributeDomain domain;

		/*! @brief Number of elements comprising one attribute value. Expected to be within [1, 3]. */
		uint8 numElements : 2;

		/*! @brief Whether the stored value is in [0, 1] ([-1, 1] for signed types).
		This attribute is for integral types only. Take uint8 for example, if this attribute is true,
		an input value of 255 will be converted to 1.0 on load; otherwise, the value is converted to
		real as-is (i.e., 255 becomes 255.0).
		*/
		uint8 shouldNormalize : 1;

		Entry();

		bool hasStrideInfo() const;
	};
	static_assert(std::is_trivially_copyable_v<Entry>);

	using AttributeMask = uint64;
	inline constexpr static auto MAX_ENTRIES = enum_size<EPrimitiveAttribute>();
	inline constexpr static auto DECLARATION_STORAGE_SIZE = sizeof(Entry) * MAX_ENTRIES;
	static_assert(MAX_ENTRIES <= sizeof_in_bits<AttributeMask>());

	/*! @brief Load and decode attribute values from an allocated entry.
	@tparam Element Storage format used to interpret each value. Must match `entry.element`.
	@param entry Non-empty attribute entry containing buffer and layout information.
	@param indices Valid indices in the entry's domain to load.
	@param out_values Destination for values in the same order as @p indices.
	*/
	template<EAttributeElement Element, std::size_t N, std::unsigned_integral Index>
	static void loadAttributeValues(
		const Entry& entry,
		const std::array<Index, N>& indices,
		std::array<math::Vector3R, N>& out_values);

	/*! @brief Directly load `float32` attribute values into real-valued vectors.
	@tparam NumElements Number of elements per attribute. Must match `entry.numElements`.
	@param entry Non-empty `float32` attribute entry containing buffer and layout information.
	@param indices Valid indices in the entry's domain to load.
	@param out_values Destination for values in the same order as @p indices.
	*/
	template<std::size_t NumElements, std::size_t N, std::unsigned_integral Index>
	static void loadAttributeValuesDirectly(
		const Entry& entry,
		const std::array<Index, N>& indices,
		std::array<math::Vector3R, N>& out_values);

	static std::size_t attributeSize(const Entry& entry);
	static std::size_t byteBufferAlignment();
	static std::size_t refineByteBufferSize(std::size_t requiredSize, std::size_t alignment);
	static std::size_t attributeStorageOffset(AttributeMask numEntries, std::size_t alignment);

	static auto makeByteBuffer(std::size_t requiredSize, std::size_t alignment)
	-> TAlignedMemoryUniquePtr<std::byte>;

	void ensureConsistentAttributeLayout() const;
	AttributeMask numEntries() const;

	/*! @brief Get entry before allocation.
	*/
	const Entry& getDeclaredEntry(EPrimitiveAttribute attribute) const;

	bool hasEntry(EPrimitiveAttribute attribute) const;

	/*! @brief Get entry after allocation.
	*/
	const Entry& getEntry(EPrimitiveAttribute attribute) const;

	/*! @brief Access to the underlying raw attribute storage.
	*/
	///@{
	std::byte* getData();
	const std::byte* getData() const;
	///@}

	Entry* getEntries();
	const Entry* getEntries() const;

	TAlignedMemoryUniquePtr<std::byte> m_byteBuffer;
	AttributeMask m_attributeMask;

#if PH_DEBUG
	bool m_isAttributeAllocated;
#endif
};

/*! @brief Mutable view for initializing an allocated attribute buffer.
The referenced buffer must outlive the writer.
*/
class IndexedAttributeBufferWriter final
{
public:
	void setAttribute(EPrimitiveAttribute attribute, std::size_t index, const math::Vector3R& value);
	void setAttribute(EPrimitiveAttribute attribute, std::size_t index, const math::Vector2R& value);
	void setAttribute(EPrimitiveAttribute attribute, std::size_t index, real value);

	/*! @brief Copies raw bytes into attribute storage.
	@param dstOffset Offset from the beginning of attribute storage.
	*/
	void setData(
		const std::byte* srcBytes,
		std::size_t numBytes,
		std::size_t dstOffset = 0);

	std::size_t byteBufferSize() const;
	std::size_t memoryUsage() const;
	std::size_t numVertices() const;
	std::size_t numFaces() const;
	std::byte* getData() const;

private:
	friend class IndexedAttributeBuffer;

	IndexedAttributeBufferWriter(
		IndexedAttributeBuffer& buffer,
		std::size_t attributeStorageSize,
		std::size_t numVertices,
		std::size_t numFaces);

	IndexedAttributeBuffer& m_buffer;
	std::size_t m_attributeStorageSize;
	std::size_t m_numVertices;
	std::size_t m_numFaces;
};

// In-header Implementations:

inline bool IndexedAttributeBuffer::Entry::hasStrideInfo() const
{
	// Cannot have partially filled stride info
	PH_ASSERT(
		(u_strideOffset != INVALID_STRIDE_VALUE && strideSize != INVALID_STRIDE_VALUE) ||
		(u_strideOffset == INVALID_STRIDE_VALUE && strideSize == INVALID_STRIDE_VALUE));

	return u_strideOffset != INVALID_STRIDE_VALUE && strideSize != INVALID_STRIDE_VALUE;
}

inline void IndexedAttributeBufferWriter::setAttribute(
	const EPrimitiveAttribute attribute,
	const std::size_t index,
	const math::Vector2R& value)
{
	setAttribute(attribute, index, math::Vector3R(value[0], value[1], 0.0_r));
}

inline void IndexedAttributeBufferWriter::setAttribute(
	const EPrimitiveAttribute attribute,
	const std::size_t index,
	const real value)
{
	setAttribute(attribute, index, math::Vector3R(value, 0.0_r, 0.0_r));
}

inline std::size_t IndexedAttributeBufferWriter::byteBufferSize() const
{
	const std::size_t alignment = IndexedAttributeBuffer::byteBufferAlignment();
	const auto requiredSize =
		IndexedAttributeBuffer::attributeStorageOffset(m_buffer.numEntries(), alignment) +
		m_attributeStorageSize;
	return IndexedAttributeBuffer::refineByteBufferSize(requiredSize, alignment);
}

inline std::size_t IndexedAttributeBufferWriter::memoryUsage() const
{
	return sizeof(IndexedAttributeBuffer) + byteBufferSize();
}

inline std::size_t IndexedAttributeBufferWriter::numVertices() const
{
	return m_numVertices;
}

inline std::size_t IndexedAttributeBufferWriter::numFaces() const
{
	return m_numFaces;
}

inline std::byte* IndexedAttributeBufferWriter::getData() const
{
	return m_buffer.getData();
}

inline bool IndexedAttributeBuffer::hasAttribute(const EPrimitiveAttribute attribute) const
{
	return hasEntry(attribute);
}

inline EAttributeDomain IndexedAttributeBuffer::getAttributeDomain(const EPrimitiveAttribute attribute) const
{
	return getEntry(attribute).domain;
}

inline bool IndexedAttributeBuffer::hasEntry(const EPrimitiveAttribute attribute) const
{
	return (m_attributeMask & math::flag_bit<AttributeMask>(enum_to_value(attribute))) != 0;
}

inline auto IndexedAttributeBuffer::getDeclaredEntry(const EPrimitiveAttribute attribute) const
-> const Entry&
{
	PH_ASSERT(!m_isAttributeAllocated);
	PH_ASSERT(hasEntry(attribute));
	return getEntries()[enum_to_value(attribute)];
}

inline auto IndexedAttributeBuffer::getEntry(const EPrimitiveAttribute attribute) const
-> const Entry&
{
	PH_ASSERT(m_isAttributeAllocated);
	PH_ASSERT(hasEntry(attribute));

	const AttributeMask attributeFlag = math::flag_bit<AttributeMask>(enum_to_value(attribute));
	const auto entryIndex = math::num_bits(m_attributeMask & (attributeFlag - 1));
	return getEntries()[entryIndex];
}

inline std::byte* IndexedAttributeBuffer::getData()
{
	PH_ASSERT(m_isAttributeAllocated);
	return m_byteBuffer.get() + attributeStorageOffset(numEntries(), byteBufferAlignment());
}

inline const std::byte* IndexedAttributeBuffer::getData() const
{
	PH_ASSERT(m_isAttributeAllocated);
	return m_byteBuffer.get() + attributeStorageOffset(numEntries(), byteBufferAlignment());
}

inline IndexedAttributeBuffer::Entry* IndexedAttributeBuffer::getEntries()
{
	return std::launder(reinterpret_cast<Entry*>(m_byteBuffer.get()));
}

inline const IndexedAttributeBuffer::Entry* IndexedAttributeBuffer::getEntries() const
{
	return std::launder(reinterpret_cast<const Entry*>(m_byteBuffer.get()));
}

inline auto IndexedAttributeBuffer::numEntries() const
-> AttributeMask
{
	return math::num_bits(m_attributeMask);
}

inline std::size_t IndexedAttributeBuffer::attributeStorageOffset(const AttributeMask numEntries, const std::size_t alignment)
{
	return math::next_power_of_2_multiple(numEntries * sizeof(Entry), alignment);
}

inline bool IndexedAttributeBuffer::AttributeDeclaration::isEmpty() const
{
	return numElements == 0;
}

}// end namespace ph

#include "Engine/Core/Intersection/DataStructure/IndexedAttributeBuffer.ipp"
