#pragma once

#include "Common/primitive_type.h"
#include "Math/TVector3.h"

#include <memory>
#include <cstddef>
#include <limits>
#include <array>

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
	VE_Float32 = 0,
	VE_Float16,
	VE_Int32,
	VE_Int16,
	VE_OctahedralUnitVec32,
	VE_OctahedralUnitVec24,

	// Special values
	NUM
};

class IndexedVertexBuffer final
{
public:
	IndexedVertexBuffer();

	void setEntry(
		EVertexAttribute attribute,
		EVertexElement   element,
		std::size_t      numElements = 0,
		bool             shouldNormalize = false);

	void allocate(std::size_t numVertices);

	math::Vector3R getAttribute(EVertexAttribute attribute, std::size_t index) const;
	std::size_t estimateMemoryUsageBytes() const;
	bool isAllocated() const;

private:
	using StrideSize = uint8;

	struct Entry final
	{
		EVertexElement element;

		/*! @brief Number of elements in this entry. Expected to be within [1, 3]. */
		uint8 numElements : 2;

		/*! @brief Whether the stored value is in [0, 1] ([-1, 1] for signed types). 
		This attribute is for integral types only. Take uint8 for example, if this attribute is true, an
		input value of 255 will be converted to 1.0 on load; otherwise, the value is converted to float
		as-is (i.e., 255 becomes 255.0).
		*/
		uint8 shouldNormalize : 1;

		StrideSize strideOffset;

		Entry();

		bool isEmpty() const;
		bool isNormalized() const;
	};

	std::array<Entry, static_cast<std::size_t>(EVertexAttribute::NUM)> m_entries;

	std::unique_ptr<std::byte[]> m_byteBuffer;
	std::size_t                  m_byteBufferSize;
	StrideSize                   m_strideSize;

	static_assert(std::numeric_limits<StrideSize>::max() >= static_cast<std::size_t>(EVertexAttribute::NUM) * (4 * 3),
		"Type <StrideSize> cannot hold max possible vertex stride size. Consider increase it.");
};

// In-header Implementations:

inline std::size_t IndexedVertexBuffer::estimateMemoryUsageBytes() const
{
	return sizeof(IndexedVertexBuffer) + m_byteBufferSize;
}

inline bool IndexedVertexBuffer::Entry::isEmpty() const
{
	return numElements == 0;
}

inline bool IndexedVertexBuffer::Entry::isNormalized() const
{
	return shouldNormalize == 1;
}

inline bool IndexedVertexBuffer::isAllocated() const
{
	return m_byteBuffer != nullptr;
}

}// end namespace ph
